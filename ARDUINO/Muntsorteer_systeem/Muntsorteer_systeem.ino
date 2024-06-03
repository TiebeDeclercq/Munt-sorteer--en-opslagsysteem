#include <LiquidCrystal_I2C.h> //LCD bibliotheek gebruiken
#include <ESP32Servo.h> //Servo bibliotheek gebruiken
#include "BluetoothSerial.h" //Bluetooth bibliotheek gebruiken
#include "pitches.h"
#include <EEPROM.h>

#define BUZZZER_PIN  27 // ESP32 pin GPIO18 connected to piezo buzzer

#define knop5cent_PIN  15 // Knop om 5 cent uit te werpen
#define knop10cent_PIN  2 // Knop om 10 cent uit te werpen
#define knop20cent_PIN  0 // Knop om 20 cent uit te werpen
#define knop50cent_PIN  23 // Knop om 50 cent uit te werpen
#define knop1euro_PIN  32 // Knop om 1 euro uit te werpen
#define knop2euro_PIN  35 // Knop om 2 euro uit te werpen

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

LiquidCrystal_I2C lcd(0x27,16,2);  //lcd object maken met LCD I2c adres 0x27

byte euro[8] = {
	0b00111,
	0b01000,
	0b11100,
	0b01000,
	0b01000,
	0b11100,
	0b01000,
	0b00111
};

String device_name = "Muntsorteer- en opslagsysteem"; //Bluetooth naam instellen

String message = ""; //Inkomende string
char incomingChar; //Inkomende char

//Ter controle voor als bluetooth niet enabled is in de esp
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT; //Object maken voor seriële bluetooth

//SDA 21, SCL 22

//Een structuur maken voor elk geldtype
struct munt {
	const uint8_t PIN;
	uint32_t aantal;
  uint32_t maxAantal;
};

munt cent5 = {33, 0, 3}; //Instantie maken voor 5 cent
munt cent10 = {16, 0, 3}; //Instantie maken voor 10 cent
munt cent20 = {17, 0, 3}; //Instantie maken voor 20 cent
munt cent50 = {5, 0, 3}; //Instantie maken voor 50 cent
munt eur1 = {18, 0, 3}; //Instantie maken voor 1 euro
munt eur2 = {19, 0, 3}; //Instantie maken voor 2 euro

bool bedragVerandert = false;
//Variabelen die zorgen voor de timing van de interrupts
unsigned long button_time = 0;  
unsigned long last_button_time = 0; 

//Pin verbonden met ST_CP van de 74HC595
int latchPin = 4;
//Pin verbonden met SH_CP van de 74HC595
int clkPin = 12;
//Pin verbonden met DS van de 74HC595
int dtPin = 14;
//Pin die verbonden is met het servo signaal
int servoPin = 13;

bool btn5c = false;
bool btn10c = false;
bool btn20c = false;
bool btn50c = false;
bool btn1eur = false;
bool btn2eur = false;

bool cent5Vol;
bool cent10Vol;
bool cent20Vol;
bool cent50Vol;
bool eur1Vol;
bool eur2Vol;



Servo myservo;  // Servo object maken om de servo te besturen

String stringOmTeVerzenden = ""; //String die gebruikt zal worden voor het verzenden

//Interrupt voor 10 cent
void IRAM_ATTR isr10c() {
  button_time = millis();
  if (button_time - last_button_time > 250)
    {
      if(cent10.aantal >= cent10.maxAantal){
        cent10Vol = true;
      }
      cent10.aantal++;
      String verzendString = "O10c" + String(cent10.aantal) + "\n";
      SerialBT.print(verzendString);
      bedragVerandert = true;
      last_button_time = button_time;
    }
}

//Interrupt voor 5 cent
void IRAM_ATTR isr5c() {
  button_time = millis();
  if (button_time - last_button_time > 250)
    {
      if(cent5.aantal >= cent5.maxAantal){
        cent5Vol = true;
      }
      cent5.aantal++;
      String verzendString = "O5c" + String(cent5.aantal) + "\n";
      SerialBT.print(verzendString);
      bedragVerandert = true;
      last_button_time = button_time;
    }
}

//Interrupt voor 20 cent
void IRAM_ATTR isr20c() {
  button_time = millis();
  if (button_time - last_button_time > 250)
    {
      if(cent20.aantal >= cent20.maxAantal){
        cent20Vol = true;
      }
      cent20.aantal++;
      String verzendString = "O20c" + String(cent20.aantal) + "\n";
      SerialBT.print(verzendString);
      bedragVerandert = true;
      last_button_time = button_time;
    }
}

//Interrupt voor 50 cent
void IRAM_ATTR isr50c() {
  button_time = millis();
  if (button_time - last_button_time > 250)
    {
      if(cent50.aantal >= cent50.maxAantal){
        cent50Vol = true;
      }
      cent50.aantal++;
      String verzendString = "O50c" + String(cent50.aantal) + "\n";
      SerialBT.print(verzendString);
      bedragVerandert = true;
      last_button_time = button_time;
    }
}

//Interrupt voor 1 euro
void IRAM_ATTR isr1eur() {
  button_time = millis();
  if (button_time - last_button_time > 250)
    {
      if(eur1.aantal >= eur1.maxAantal){
        eur1Vol = true;
      }
      eur1.aantal++;
      String verzendString = "O100c" + String(eur1.aantal) + "\n";
      SerialBT.print(verzendString);
      bedragVerandert = true;
      last_button_time = button_time;
    }
}

//Interrupt voor 2 euro
void IRAM_ATTR isr2eur() {
  button_time = millis();
  if (button_time - last_button_time > 250)
    {
      if(eur2.aantal >= eur2.maxAantal){
        eur2Vol = true;
      }
      eur2.aantal++;
      String verzendString = "O200c" + String(eur2.aantal) + "\n";
      SerialBT.print(verzendString);
      bedragVerandert = true;
      last_button_time = button_time;
    }
}

void setup() {
  lcd.init();
  lcd.createChar(0, euro);
  lcd.clear();         
  lcd.backlight();   
  updateLCD("GIPTiebeDeclercq", "test");  

  SerialBT.begin(device_name); //Bluetooth naam instellen
  Serial.begin(115200); //Communicatie beginnen voor in de serial monitor te debuggen

	pinMode(cent10.PIN, INPUT);
  pinMode(cent5.PIN, INPUT);
	pinMode(cent20.PIN, INPUT);
  pinMode(cent50.PIN, INPUT);
  pinMode(eur1.PIN, INPUT);
  pinMode(eur2.PIN, INPUT);

  pinMode(knop5cent_PIN, INPUT);
  pinMode(knop10cent_PIN, INPUT);
  pinMode(knop20cent_PIN, INPUT);
  pinMode(knop50cent_PIN, INPUT);
  pinMode(knop1euro_PIN, INPUT);
  pinMode(knop2euro_PIN, INPUT);
  
	attachInterrupt(cent10.PIN, isr10c, FALLING);
  attachInterrupt(cent5.PIN, isr5c, FALLING);
  attachInterrupt(cent20.PIN, isr20c, FALLING);
  attachInterrupt(cent50.PIN, isr50c, FALLING);
  attachInterrupt(eur1.PIN, isr1eur, FALLING);
  attachInterrupt(eur2.PIN, isr2eur, FALLING);

  // de 74HC595 pinnen als uitgangen instellen
  pinMode(latchPin, OUTPUT);    //ST_CP pin van de 74HC595
  pinMode(clkPin, OUTPUT);      //SH_CP pin van de 74HC595
  pinMode(dtPin, OUTPUT);       //DS pin van de 74HC595

  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BUZZZER_PIN, melody[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(BUZZZER_PIN);
  }

  // Alle timer allocaten voor de servo
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3); 
	myservo.setPeriodHertz(50);    // standaard 50 hz servo
	myservo.attach(servoPin, 500, 2400); // Servo verbinden met de servopin

}

void loop() {
  if (SerialBT.available()) { //Enkel als je bluetooth hebt uitvoeren
    char incomingChar = SerialBT.read(); //Inkomende data lezen
    if (incomingChar != '\n'){ //Nog niet het einde van de verzonden data
      message += String(incomingChar); //Char toevoegen aan de string
    }
    else{ //Einde verzonden data
      dataVerwerken(message); //Deze data verwerken
      message = "";
    }
    Serial.write(incomingChar); //In de serial monitor de data sturen voor debuggen 
  }
  delay(20);
  if(bedragVerandert){
    updateLCD("Totale bedrag:", String(totaleBedrag())+" euro");
    bedragVerandert = false;
  }

  if(digitalRead(knop5cent_PIN) && !btn5c){
    dispenseCoin(5);
    btn5c = true;
  }
  else if(!digitalRead(knop5cent_PIN) && btn5c){
    btn5c = false;
  }
  if (digitalRead(knop10cent_PIN) && !btn10c){
    dispenseCoin(10);
    btn10c = true;
  }else if(!digitalRead(knop10cent_PIN) && btn10c){
    btn10c = false;
  }
  /*if (digitalRead(knop20cent_PIN)){
    Serial.write("knop20c");
    dispenseCoin(20);
  }*/
  if (digitalRead(knop50cent_PIN) && !btn50c){
    dispenseCoin(50);
    btn50c= true;
  }else if(!digitalRead(knop50cent_PIN) && btn50c){
    btn50c= false;
  }
  if (digitalRead(knop1euro_PIN) && !btn1eur){
    dispenseCoin(100);
    btn1eur = true;
  }else if(!digitalRead(knop1euro_PIN) && btn1eur){
    btn1eur = false;
  }
  if (digitalRead(knop2euro_PIN) && !btn2eur){
    dispenseCoin(200);
    btn2eur = true;
  }else if(!digitalRead(knop2euro_PIN) && btn2eur){
    btn2eur = false;
  }

  if(cent5Vol){
    dispenseCoin(5);
    cent5Vol = false;
  }
  if(cent10Vol){
    dispenseCoin(10);
    cent10Vol = false;
  }
  if(cent20Vol){
    dispenseCoin(20);
    cent20Vol = false;
  }
  if(cent50Vol){
    dispenseCoin(50);
    cent50Vol = false;
  }
  if(eur1Vol){
    dispenseCoin(100);
    eur1Vol = false;
  }
  if(eur2Vol){
    dispenseCoin(200);
    eur2Vol = false;
  }
}

float totaleBedrag(){
  return (eur2.aantal * 2 + eur1.aantal + cent50.aantal * 0.5 + cent20.aantal * 0.2 + cent10.aantal * 0.1 + cent5.aantal * 0.05);
}

void updateLCD(String firstline, String secondLine){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(firstline);
  lcd.setCursor(0, 1);
  lcd.write(0);
  lcd.setCursor(1, 1);
  lcd.print(secondLine);
}

//Data verwerken
void dataVerwerken(String verzondenData){
  char firstLetter = verzondenData.charAt(0); //Eerste karakter inlezen
  if(firstLetter == 's'){ //Geldtype uitwerpen
    String derestVanDeString = verzondenData.substring(1);
    if (derestVanDeString == "5"){
      dispenseCoin(5);
    }
    else if (derestVanDeString == "10"){
      dispenseCoin(10);
    }
    else if (derestVanDeString == "20"){
    dispenseCoin(20);
    }
    else if (derestVanDeString == "50"){
    dispenseCoin(50);
    }
    else if (derestVanDeString == "1"){
    dispenseCoin(100);
    }
      else if (derestVanDeString == "2"){
    dispenseCoin(200);
    }
  }
  else if(firstLetter == 'l'){ //LCD aan/af
    char secondLetter = verzondenData.charAt(1);
    if(secondLetter == 's'){
      String tekst = verzondenData.substring(2);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(tekst);
      Serial.println(tekst);
    }
    else{
      String derestVanDeString = verzondenData.substring(1);
      if(derestVanDeString == "1"){ //LCD aan
        lcd.backlight();
      }
      else{ //LCD af
        lcd.noBacklight();
      }
    }
  }
  else if (message == "v"){ //Programma is net verbonden
    //Alle huidige geldwaarden doorsturen
    SerialBT.print("O200c" + String(eur2.aantal) + "\n");
    delay(50);
    SerialBT.print("O100c" + String(eur1.aantal) + "\n");
    delay(50);
    SerialBT.print("O50c" + String(cent50.aantal) + "\n");
    delay(50);
    SerialBT.print("O20c" + String(cent20.aantal) + "\n");
    delay(50);
    SerialBT.print("O10c" + String(cent10.aantal) + "\n");
    delay(50);
    SerialBT.print("O5c" + String(cent5.aantal) + "\n");
  }
}
//Een geldtype uitwerpen
void dispenseCoin(int amount){
  switch(amount){
    case 5: //5 cent
      if(cent5.aantal > 0){ //Enkel uitvoeren als je geld van deze soort hebt
        updateShiftRegister(0b01000000);
        cent5.aantal--;
        stringOmTeVerzenden = "O5c" + String(cent5.aantal) + "\n";
        SerialBT.print(stringOmTeVerzenden); //Versturen via bluetooth
        Serial.println(stringOmTeVerzenden);  //Serieel in de monitor versturen voor debug redenen
        //Daarna de servo aansturen
        myservo.write(180);
        delay(400); //Zorgen dat er genoeg tijd is voor de servo om te draaien
        myservo.write(0);
        bedragVerandert = true;
      }
      break;
    case 10: //10 cent
      if(cent10.aantal > 0){
      updateShiftRegister(0b10000000);
      cent10.aantal--;
      stringOmTeVerzenden = "O10c" + String(cent10.aantal) + "\n";
        SerialBT.print(stringOmTeVerzenden); //Versturen via bluetooth
        Serial.println(stringOmTeVerzenden);  //Serieel in de monitor versturen voor debug redenen
        //Daarna de servo aansturen
        myservo.write(180);
        delay(400); //Zorgen dat er genoeg tijd is voor de servo om te draaien
        myservo.write(0);
        bedragVerandert = true;
      }
      break;
    case 20: //20 cent
    if(cent20.aantal > 0){
      updateShiftRegister(0b00100000);
      cent20.aantal--;
      stringOmTeVerzenden = "O20c" + String(cent20.aantal) + "\n";
              SerialBT.print(stringOmTeVerzenden); //Versturen via bluetooth
        Serial.println(stringOmTeVerzenden);  //Serieel in de monitor versturen voor debug redenen
        //Daarna de servo aansturen
        myservo.write(180);
        delay(400); //Zorgen dat er genoeg tijd is voor de servo om te draaien
        myservo.write(0);
        bedragVerandert = true;
      }
      break;
    case 50: //50 cent
    if(cent50.aantal > 0){
      updateShiftRegister(0b00001000);
      cent50.aantal--;
      stringOmTeVerzenden = "O50c" + String(cent50.aantal) + "\n";
              SerialBT.print(stringOmTeVerzenden); //Versturen via bluetooth
        Serial.println(stringOmTeVerzenden);  //Serieel in de monitor versturen voor debug redenen
        //Daarna de servo aansturen
        myservo.write(180);
        delay(400); //Zorgen dat er genoeg tijd is voor de servo om te draaien
        myservo.write(0);
        bedragVerandert = true;
      }
      break;
    case 100: // 1 euro
    if(eur1.aantal > 0){
      updateShiftRegister(0b00010000);
      eur1.aantal--;
      stringOmTeVerzenden = "O100c" + String(eur1.aantal) + "\n";
              SerialBT.print(stringOmTeVerzenden); //Versturen via bluetooth
        Serial.println(stringOmTeVerzenden);  //Serieel in de monitor versturen voor debug redenen
        //Daarna de servo aansturen
        myservo.write(180);
        delay(400); //Zorgen dat er genoeg tijd is voor de servo om te draaien
        myservo.write(0);
        bedragVerandert = true;
      }
      break;
    case 200: // 2 euro
    if(eur2.aantal > 0){
      updateShiftRegister(0b00000100);
      eur2.aantal--;
      stringOmTeVerzenden = "O200c" + String(eur2.aantal) + "\n";
              SerialBT.print(stringOmTeVerzenden); //Versturen via bluetooth
        Serial.println(stringOmTeVerzenden);  //Serieel in de monitor versturen voor debug redenen
        //Daarna de servo aansturen
        myservo.write(180);
        delay(400); //Zorgen dat er genoeg tijd is voor de servo om te draaien
        myservo.write(0);
        bedragVerandert = true;
      }
      break;
  }
}

//Deze functie is voor het aansturen van het schuifregister
void updateShiftRegister(int uitgangen)
{
  digitalWrite(latchPin, LOW);
  shiftOut(dtPin, clkPin, LSBFIRST, uitgangen);
  digitalWrite(latchPin, HIGH);
}