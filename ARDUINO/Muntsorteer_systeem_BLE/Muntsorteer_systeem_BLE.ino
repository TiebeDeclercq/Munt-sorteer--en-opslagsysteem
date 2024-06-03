//Muntsorteer- en opslagsysteem
//(C)Tiebe Declercq 2024

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <LiquidCrystal_I2C.h> //LCD bibliotheek gebruiken
#include <ESP32Servo.h> //Servo bibliotheek gebruiken
#include <EEPROM.h> 
#include "pitches.h"

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E" //RX characteristic UUID
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E" //TX characteristic UUID

#define BUZZZER_PIN  27 // ESP32 pin GPIO18 connected to piezo buzzer

#define knop5cent_PIN  15 // Knop om 5 cent uit te werpen
#define knop10cent_PIN  2 // Knop om 10 cent uit te werpen
#define knop20cent_PIN  0 // Knop om 20 cent uit te werpen
#define knop50cent_PIN  23 // Knop om 50 cent uit te werpen
#define knop1euro_PIN  32 // Knop om 1 euro uit te werpen
#define knop2euro_PIN  35 // Knop om 2 euro uit te werpen

LiquidCrystal_I2C lcd(0x27,16,2);  //lcd object maken met LCD I2c adres 0x27

byte euro[8] = { //Speciaal euro teken voor LCD display
	0b00111,
	0b01000,
	0b11100,
	0b01000,
	0b01000,
	0b11100,
	0b01000,
	0b00111
};

//SDA 21, SCL 22

//Een structuur maken voor elk geldtype
struct munt {
	const uint8_t PIN;
	uint32_t aantal;
  uint32_t maxAantal;
};

munt cent5 = {33, 0, 20}; //Instantie maken voor 5 cent
munt cent10 = {16, 0, 20}; //Instantie maken voor 10 cent
munt cent20 = {17, 0, 20}; //Instantie maken voor 20 cent
munt cent50 = {5, 0, 20}; //Instantie maken voor 50 cent
munt eur1 = {18, 0, 20}; //Instantie maken voor 1 euro
munt eur2 = {19, 0, 20}; //Instantie maken voor 2 euro

bool bedragVerandert = false; //Bijhouden wanneer de lcd moet updaten naar ene nieuw bedrag

bool cent5changed = false; //Het 5 cent aantal is verandert 
bool cent10changed = false; //Het 10 cent aantal is verandert 
bool cent20changed = false; //Het 20 cent aantal is verandert 
bool cent50changed = false; //Het 50 cent aantal is verandert 
bool eur1changed = false; //Het 1 euro aantal is verandert 
bool eur2changed = false; //Het 2 euro aantal is verandert 

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

bool customLcdModus = false; //LCD niet met de geldwaarde updaten
bool nietInlatenModus = false; //Niets meer in het systeem laten

Servo myservo;  // Servo object maken om de servo te besturen

String stringOmTeVerzenden = ""; //String die gebruikt zal worden voor het verzenden

bool boot = true; //1x bij opstart

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void dataVerwerken(String);

//Interrupt voor 10 cent
void IRAM_ATTR isr10c() {
  button_time = millis(); //Gebruik maken van millis zodat het niet onderbroken wordt
  if (button_time - last_button_time > 100) //Dender op de interrupt vermijden
    {
      cent10.aantal++;
      cent10changed = true;
      bedragVerandert = true;
      last_button_time = button_time;
    }
}

//Interrupt voor 5 cent
void IRAM_ATTR isr5c() {
  button_time = millis();
  if (button_time - last_button_time > 100)
    {
      cent5.aantal++;
      cent5changed = true;
      bedragVerandert = true;
      last_button_time = button_time;
    }
}

//Interrupt voor 20 cent
void IRAM_ATTR isr20c() {
  button_time = millis();
  if (button_time - last_button_time > 100)
    {
      cent20.aantal++;
      cent20changed = true;
      bedragVerandert = true;
      last_button_time = button_time;
    }
}

//Interrupt voor 50 cent
void IRAM_ATTR isr50c() {
  button_time = millis();
  if (button_time - last_button_time > 100)
    {
      cent50.aantal++;
      cent50changed = true;
      bedragVerandert = true;
      last_button_time = button_time;
    }
}

//Interrupt voor 1 euro
void IRAM_ATTR isr1eur() {
  button_time = millis();
  if (button_time - last_button_time > 50)
    {
      eur1.aantal++;
      eur1changed = true;
      bedragVerandert = true;
      last_button_time = button_time;
    }
}

//Interrupt voor 2 euro
void IRAM_ATTR isr2eur() {
  button_time = millis();
  if (button_time - last_button_time > 50)
    {
      eur2.aantal++;
      eur2changed = true;
      bedragVerandert = true;
      last_button_time = button_time;
    }
}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();
    const char* cString = rxValue.c_str(); // Convert std::string to const char*
    String arduinoString = String(cString); // Convert const char* to Arduino String
    dataVerwerken(arduinoString);
    }
};


void setup() {
  lcd_INIT();
  Serial.begin(115200); //Serial monitor kunnen zien
  pinMode(BUZZZER_PIN, OUTPUT); //Buzzer ingang    
  irsensor_INIT();
  button_INIT();
	interrupt_INIT();
  schuifregister_INIT();
  servo_INIT();
  ble_INIT();
}

//Initialisatie voor je lcd
void lcd_INIT(){
  lcd.init();
  lcd.createChar(0, euro); //Custom char voor euro teken maken
  lcd.clear();         
  lcd.backlight();   
  updateLCD("GIPTiebeDeclercq", "0.0");  
}

//Initialisatie voor al je infrarood sensoren
void irsensor_INIT(){
  pinMode(cent10.PIN, INPUT);
  pinMode(cent5.PIN, INPUT);
	pinMode(cent20.PIN, INPUT);
  pinMode(cent50.PIN, INPUT);
  pinMode(eur1.PIN, INPUT);
  pinMode(eur2.PIN, INPUT);
}

//Initialisatie voor al je drukknoppen
void button_INIT(){
  pinMode(knop5cent_PIN, INPUT);
  pinMode(knop10cent_PIN, INPUT);
  pinMode(knop20cent_PIN, INPUT);
  pinMode(knop50cent_PIN, INPUT);
  pinMode(knop1euro_PIN, INPUT);
  pinMode(knop2euro_PIN, INPUT);
}

//Initialisatie voor je interrupt
void interrupt_INIT(){
	attachInterrupt(cent10.PIN, isr10c, FALLING);
  attachInterrupt(cent5.PIN, isr5c, FALLING);
  attachInterrupt(cent20.PIN, isr20c, FALLING);
  attachInterrupt(cent50.PIN, isr50c, FALLING);
  attachInterrupt(eur1.PIN, isr1eur, FALLING);
  attachInterrupt(eur2.PIN, isr2eur, FALLING);
}

//Initialisatie voor je schuifregister
void schuifregister_INIT(){
  // de 74HC595 pinnen als uitgangen instellen
  pinMode(latchPin, OUTPUT);    //ST_CP pin van de 74HC595
  pinMode(clkPin, OUTPUT);      //SH_CP pin van de 74HC595
  pinMode(dtPin, OUTPUT);       //DS pin van de 74HC595
}

//Initialistaie voor je servo
void servo_INIT(){
  // timer allocaten voor de servo
	ESP32PWM::allocateTimer(2);
	myservo.setPeriodHertz(50);    // standaard 50 hz servo
	myservo.attach(servoPin, 500, 2400); // Servo verbinden met de servopin
}

//Initialisatie voor je bluetooth low energy
void ble_INIT(){
    // Create the BLE Device
  BLEDevice::init("Muntsorteer- en opslagsysteem");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
										CHARACTERISTIC_UUID_TX,
										BLECharacteristic::PROPERTY_NOTIFY
									);
                      
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
											 CHARACTERISTIC_UUID_RX,
											BLECharacteristic::PROPERTY_WRITE
										);

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  eersteKeerVerbonden();
  disconnecting();
  lcdUpdaten();
  drukknoppenLezen();
  maximaalAantalGeld();
  geldBedragPollen();
}

//De verbinding stoppen met het muntsorteersysteem
void disconnecting(){
  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising"); 
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }
}

//De lcd updaten
void lcdUpdaten(){
  if(bedragVerandert && !customLcdModus){ //De LCD updaten met de nieuwe geldwaarde
    updateLCD("Totale bedrag:", String(totaleBedrag())+" euro");
    bedragVerandert = false;
  }
}

//Bij het begin van de verbinding met het muntsorteersysteem
void eersteKeerVerbonden(){
  if (deviceConnected) {
    if(boot == true){ //Als het de opstart is
      //Het muntsysteem zal een korte bieb laten horen
      delay(400);
      digitalWrite(BUZZZER_PIN, HIGH);
      delay(200);
      digitalWrite(BUZZZER_PIN, LOW);
      delay(600);
      startWaardenVersturen();
      boot = false;
     }
  }
  else{
    boot = true; //Als het niet verbonden is is boot terug true
  }
}

//Alle drukknoppen inlezen
void drukknoppenLezen(){
    //Hier zal ik alle drukknoppen inlezen
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
  if (digitalRead(knop20cent_PIN) && !btn20c){
    dispenseCoin(20);
    btn20c = true;
  }else if(!digitalRead(knop20cent_PIN) && btn20c){
    btn20c= false;
  }
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
}

//Enkel tot het maximaal geldaantal toelaten
void maximaalAantalGeld(){
  //Als het maximaal geldaantallen overschreden word zal het terug uitgeworpen worden
  if(cent5.aantal > cent5.maxAantal){
    delay(200);
    dispenseCoin(5);
  }
  else if(cent10.aantal > cent10.maxAantal){
    delay(200);
    dispenseCoin(10);
  }
  else if(cent20.aantal > cent20.maxAantal){
    delay(200);
    dispenseCoin(20);
  }
  else if(cent50.aantal > cent50.maxAantal){
    delay(200);
   dispenseCoin(50);
  }
  else if(eur1.aantal > eur1.maxAantal){
    delay(200);
   dispenseCoin(100);
  }
  else if(eur2.aantal > eur2.maxAantal){
    delay(200);
   dispenseCoin(200);
  }
}

//Wanneer er een geldbedrag verandert
void geldBedragPollen(){
 //Wanneer een geldbedrag verandert is zal hij deze nieuwe waarde versturen
  if(cent5changed){
    String verzendString = "O5c" + String(cent5.aantal);
    dataVersturen(verzendString);
    cent5changed = false;
    if(nietInlatenModus){
      dispenseCoin(5);
    }
  }
   if(cent10changed){
    String verzendString = "O10c" + String(cent10.aantal);
    dataVersturen(verzendString);
    cent10changed = false;
    if(nietInlatenModus){
      dispenseCoin(10);
    }
  }
   if(cent20changed){
    String verzendString = "O20c" + String(cent20.aantal);
    dataVersturen(verzendString);
    cent20changed = false;
    if(nietInlatenModus){
      dispenseCoin(20);
    }
  }
   if(cent50changed){
    String verzendString = "O50c" + String(cent50.aantal);
    dataVersturen(verzendString);
    cent50changed = false;
    if(nietInlatenModus){
      dispenseCoin(50);
    }
  }
   if(eur1changed){
    String verzendString = "O100c" + String(eur1.aantal);
    dataVersturen(verzendString);
    eur1changed = false;
    if(nietInlatenModus){
      dispenseCoin(100);
    }
  }
   if(eur2changed){
    String verzendString = "O200c" + String(eur2.aantal);
    dataVersturen(verzendString);
    eur2changed = false;
    if(nietInlatenModus){
      dispenseCoin(200);
    }
  }
}

//Functie om het totale geldbedrag te berekenen
float totaleBedrag(){
  return (eur2.aantal * 2 + eur1.aantal + cent50.aantal * 0.5 + cent20.aantal * 0.2 + cent10.aantal * 0.1 + cent5.aantal * 0.05);
}

//Als het systeem net verbonden is zal hij alle startwaarden doorsturen
void startWaardenVersturen(){
  dataVersturen("O200c" + String(eur2.aantal));
  dataVersturen("O100c" + String(eur1.aantal));
  dataVersturen("O50c" + String(cent50.aantal));
  dataVersturen("O20c" + String(cent20.aantal));
  dataVersturen("O10c" + String(cent10.aantal));
  dataVersturen("O5c" + String(cent5.aantal));
  dataVersturen("OM200c" + String(eur2.maxAantal));
  dataVersturen("OM100c" + String(eur1.maxAantal));
  dataVersturen("OM50c" + String(cent50.maxAantal));
  dataVersturen("OM20c" + String(cent20.maxAantal));
  dataVersturen("OM10c" + String(cent10.maxAantal));
  dataVersturen("OM5c" + String(cent5.maxAantal));
}

//LCD updaten met de parameters die je hier kan meegeven
void updateLCD(String firstline, String secondLine){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(firstline);
  lcd.setCursor(0, 1);
  lcd.write(0);
  lcd.setCursor(1, 1);
  lcd.print(secondLine);
}

//De verzonden data verwerken
void dataVerwerken(String verzondenData){
  Serial.println(verzondenData);
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
    else if(secondLetter == 'm'){
      String derestVanDeString = verzondenData.substring(2);
      if(derestVanDeString == "1"){
        customLcdModus = true;
      }
      else{
        customLcdModus = false;
      }
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
  else if (firstLetter == 'o') // Je ontvangt een nieuw geld aantal
  {
    if(verzondenData.charAt(1) == 'm'){ //Het is een maximaal geldaantal
      String geldType = verzondenData.substring(2, verzondenData.lastIndexOf('c')); // Eerst het type van het geld bekijken
      int maxGeldAantal = verzondenData.substring(verzondenData.lastIndexOf('c') + 1).toInt(); // Vervolgens het aantal van dit geldtype omzetten
          if(maxGeldAantal > 0){
    if (geldType == "5") {
        cent5.maxAantal = maxGeldAantal;
        dataVersturen("OM5c" + String(cent5.maxAantal));

    } else if (geldType == "10") {
        cent10.maxAantal = maxGeldAantal;
        dataVersturen("OM10c" + String(cent10.maxAantal));
    } else if (geldType == "20") {
        cent20.maxAantal = maxGeldAantal;
        dataVersturen("OM20c" + String(cent20.maxAantal));
    } else if (geldType == "50") {
        cent50.maxAantal = maxGeldAantal;
        dataVersturen("OM50c" + String(cent50.maxAantal));
    } else if (geldType == "100") {
        eur1.maxAantal = maxGeldAantal;
        dataVersturen("OM100c" + String(eur1.maxAantal));
    } else if (geldType == "200") {
        eur2.maxAantal = maxGeldAantal;
        dataVersturen("OM200c" + String(eur2.maxAantal));
    }
    }
    }
    String geldType = verzondenData.substring(1, verzondenData.lastIndexOf('c')); // Eerst het type van het geld bekijken
    int geldAantal = verzondenData.substring(verzondenData.lastIndexOf('c') + 1).toInt(); // Vervolgens het aantal van dit geldtype omzetten
    if(geldAantal >= 0){
    if (geldType == "5") {
        cent5.aantal = geldAantal;
        cent5changed = true;
    } else if (geldType == "10") {
        cent10.aantal = geldAantal;
        cent10changed = true;
    } else if (geldType == "20") {
        cent20.aantal = geldAantal;
        cent20changed = true;
    } else if (geldType == "50") {
        cent50.aantal = geldAantal;
        cent50changed = true;
    } else if (geldType == "100") {
        eur1.aantal = geldAantal;
        eur1changed = true;
    } else if (geldType == "200") {
        eur2.aantal = geldAantal;
        eur2changed = true;
    }
    }
    bedragVerandert = true;
  }
  else if(firstLetter == 'u'){ //Een zelfgekozen geldbedrag uitwerpen
    int geldAantal = verzondenData.substring(1).toInt(); // Vervolgens het aantal van dit geldtype omzetten
    geld_Aantal_Uitwerpen(geldAantal);
  }
  else if(firstLetter == 'd'){ //Doorlaatmodus
    String derestVanDeString = verzondenData.substring(1);
    if(derestVanDeString == "1"){
      nietInlatenModus = true;
    }
    else{
      nietInlatenModus = false;
    }
  }
}

//Een geldtype uitwerpen
void dispenseCoin(int amount){
  switch(amount){
    case 5: //5 cent
      if(cent5.aantal > 0){ //Enkel uitvoeren als je geld van deze soort hebt
        updateShiftRegister(0b01000000);
        cent5.aantal--;
        stringOmTeVerzenden = "O5c" + String(cent5.aantal);
        dataVersturen(stringOmTeVerzenden); //Versturen via bluetooth
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
      stringOmTeVerzenden = "O10c" + String(cent10.aantal);
        dataVersturen(stringOmTeVerzenden); //Versturen via bluetooth
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
      stringOmTeVerzenden = "O20c" + String(cent20.aantal);
              dataVersturen(stringOmTeVerzenden); //Versturen via bluetooth
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
      stringOmTeVerzenden = "O50c" + String(cent50.aantal);
              dataVersturen(stringOmTeVerzenden); //Versturen via bluetooth
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
      stringOmTeVerzenden = "O100c" + String(eur1.aantal);
              dataVersturen(stringOmTeVerzenden); //Versturen via bluetooth
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
      stringOmTeVerzenden = "O200c" + String(eur2.aantal);
        dataVersturen(stringOmTeVerzenden); //Versturen via bluetooth
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

void geld_Aantal_Uitwerpen(int bedrag){
	// Bereken de aantallen munten
	while (bedrag >= 200 && eur2.aantal > 0) {
    delay(200);
		dispenseCoin(200);
		bedrag -= 200;
	}
	while (bedrag >= 100 && eur1.aantal > 0) {
    delay(200);
		dispenseCoin(100);
		bedrag -= 100;
	}
	while (bedrag >= 50 && cent50.aantal > 0) {
    delay(200);
		dispenseCoin(50);
		bedrag -= 50;
	}
	while (bedrag >= 20 && cent20.aantal > 0) {
    delay(200);
		dispenseCoin(20);
		bedrag -= 20;
	}
	while (bedrag >= 10 && cent10.aantal > 0) {
    delay(200);
		dispenseCoin(10);
		bedrag -= 10;
	}
	while (bedrag >= 5 && cent5.aantal > 0) {
    delay(200);
		dispenseCoin(5);
		bedrag -= 5;
	}
}

//Deze functie is voor het aansturen van het schuifregister
void updateShiftRegister(int uitgangen)
{
  digitalWrite(latchPin, LOW);
  shiftOut(dtPin, clkPin, LSBFIRST, uitgangen);
  digitalWrite(latchPin, HIGH);
}

//Data versturen via bluetooth low energy
void dataVersturen(String toSend){
    std::string stdToSend = toSend.c_str(); // Convert String to std::string
    pTxCharacteristic->setValue(stdToSend);
    pTxCharacteristic->notify();
    delay(10); // Add delay to avoid congestion
}
