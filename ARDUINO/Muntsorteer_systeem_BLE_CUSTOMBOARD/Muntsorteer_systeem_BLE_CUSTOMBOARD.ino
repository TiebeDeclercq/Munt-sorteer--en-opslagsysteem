//Muntsorteer- en opslagsysteem
//(C)Tiebe Declercq 2024

#include <BLEDevice.h> //Alle bibliotheken voor BLE
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <LiquidCrystal_I2C.h> //LCD bibliotheek gebruiken
#include <ESP32Servo.h> //Servo bibliotheek gebruiken
#include <EEPROM.h>  //EEPROM bibliotheek gebruiken 

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E" //RX characteristic UUID
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E" //TX characteristic UUID

#define BUZZZER_PIN  42 // ESP32 pin GPIO42 connected to piezo buzzer
#define EEPROM_SIZE 60 //Voorbehouden plaats in EEPROM 

LiquidCrystal_I2C lcd(0x27,16,2);  //lcd object maken met LCD I2c adres 0x27

//Al deze bitmaps zijn gemaakt met https://omerk.github.io/lcdchargen/

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

byte blLT[8] = { //Bluetooth teken linksboven
	0b00000,
	0b00001,
	0b00001,
	0b00001,
	0b01001,
	0b00101,
	0b00011,
	0b00001
};

byte blRT[8] = { //Bluetooth teken rechtsboven
	0b00000,
	0b10000,
	0b01000,
	0b00100,
	0b00010,
	0b00100,
	0b01000,
	0b10000
};

byte blLB[8] = { //Bluetooth teken linksbeneden
	0b00000,
	0b00001,
	0b00011,
	0b00101,
	0b01001,
	0b00001,
	0b00001,
	0b00000
};

byte blRB[8] = { //Bluetooth teken rechtsbeneden
	0b10000,
	0b01000,
	0b00100,
	0b00010,
	0b00100,
	0b01000,
	0b10000,
	0b00000
};

//Een structuur maken voor elk geldtype
struct munt {
	const uint8_t irPIN; //Pin van de IRsensor
  const uint8_t btnPIN; //Pin van de IRsensor
	uint32_t aantal; //Houdige aantal munten
  uint32_t maxAantal; //Maximum aantal munten
  uint32_t adresAantal; //Adres aantal munten EEPROM
  uint32_t adresMaxAantal; //Adres maximalewaarde EEPROM
};

munt cent5 = {12, 2, 0, 20, 0, 4}; //Instantie maken voor 5 cent
munt cent10 = {11, 1, 0, 20, 8, 12}; //Instantie maken voor 10 cent
munt cent20 = {13, 17, 0, 20, 16, 20}; //Instantie maken voor 20 cent
munt cent50 = {15, 21, 0, 20, 24, 28}; //Instantie maken voor 50 cent
munt eur1 = {14, 18, 0, 20, 32, 36}; //Instantie maken voor 1 euro
munt eur2 = {16, 41, 0, 20, 40, 44}; //Instantie maken voor 2 euro

bool bedragVerandert = false; //Bijhouden wanneer de lcd moet updaten naar een nieuw bedrag

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
int latchPin = 6;
//Pin verbonden met SH_CP van de 74HC595
int clkPin = 7;
//Pin verbonden met DS van de 74HC595
int dtPin = 5;
//Pin die verbonden is met het pwm signaal voor de servo
int servoPin = 10;

bool btn5c = false;  //5 cent knop ingedrukt
bool btn10c = false; //10 cent knop ingedrukt
bool btn20c = false; //20 cent knop ingedrukt
bool btn50c = false; //50 cent knop ingedrukt
bool btn1eur = false; //1 euro knop ingedrukt
bool btn2eur = false; //2 euro knop ingedrukt

bool customLcdTekstUpdaten = false;
String customLcdTekst;
bool customLcdModus = false; //LCD niet met de geldwaarde updaten
bool nietInlatenModus = false; //Niets meer in het systeem laten
bool knoppenWerken = true; //Je kan de knoppen in de opstelling gebruiken

Servo myservo;  // Servo object maken om de servo te besturen

bool boot = true; //1x bij opstart

void dataVerwerken(String);

//Interrupt voor 10 cent
void IRAM_ATTR isr10c() {
  button_time = millis(); //Gebruik maken van millis zodat het programma niet onderbroken wordt
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
  EEPROM.begin(EEPROM_SIZE); //EEPROM starten
  leesWaarden(); //Waarden van de EEPROM inlezen
  lcd_INIT(); //LCD initialiseren
  Serial.begin(115200); //Serial monitor kunnen zien
  pinMode(BUZZZER_PIN, OUTPUT); //Buzzer als ingang zetten    
  irsensor_INIT(); //IR sensoren initialiseren
  button_INIT(); //Knoppen initialiseren
	interrupt_INIT(); //Inturrupts initialiseren
  schuifregister_INIT(); //Schuifregister initialiseren
  servo_INIT(); //Servo initialiseren
  ble_INIT(); //Bluetooth initialiseren
}

//Alle waarden van de EEPROM inlezen
void leesWaarden(){
  /*EEPROM.writeUInt(cent5.adresAantal, 0);
  EEPROM.writeUInt(cent10.adresAantal, 0);
  EEPROM.writeUInt(cent20.adresAantal, 0 );
  EEPROM.writeUInt(cent50.adresAantal, 0);
  EEPROM.writeUInt(eur1.adresAantal, 0);
  EEPROM.writeUInt(eur2.adresAantal, 0);
  EEPROM.writeUInt(cent5.adresMaxAantal, 20);
  EEPROM.writeUInt(cent10.adresMaxAantal, 20);
  EEPROM.writeUInt(cent20.adresMaxAantal, 20);
  EEPROM.writeUInt(cent50.adresMaxAantal, 20);
  EEPROM.writeUInt(eur1.adresMaxAantal, 20);
  EEPROM.writeUInt(eur2.adresMaxAantal, 20);
  EEPROM.commit();*/
  cent5.aantal = EEPROM.readUInt(cent5.adresAantal);
  cent5.maxAantal = EEPROM.readUInt(cent5.adresMaxAantal);
  cent10.aantal = EEPROM.readUInt(cent10.adresAantal);
  cent10.maxAantal = EEPROM.readUInt(cent10.adresMaxAantal);
  cent20.aantal = EEPROM.readUInt(cent20.adresAantal);
  cent20.maxAantal = EEPROM.readUInt(cent20.adresMaxAantal);
  cent50.aantal = EEPROM.readUInt(cent50.adresAantal);
  cent50.maxAantal = EEPROM.readUInt(cent50.adresMaxAantal);
  eur1.aantal = EEPROM.readUInt(eur1.adresAantal);
  eur1.maxAantal = EEPROM.readUInt(eur1.adresMaxAantal);
  eur2.aantal = EEPROM.readUInt(eur2.adresAantal);
  eur2.maxAantal = EEPROM.readUInt(eur2.adresMaxAantal);
  bedragVerandert = true; //Zorgen dat de lcd geüpdatet wordt met de nieuwe waarden
}

//Initialisatie voor je lcd
void lcd_INIT(){
  lcd.init(); //Lcd initialiseren
  lcd.createChar(0, euro); //Custom char voor euro teken maken
  lcd.createChar(1, blLT); //Custom char voor bluetooth teken maken
  lcd.createChar(2, blLB); //Custom char voor bluetooth teken maken
  lcd.createChar(3, blRT); //Custom char voor bluetooth teken maken
  lcd.createChar(4, blRB); //Custom char voor bluetooth teken maken
  lcd.clear();  
  lcd.backlight(); //Backlight aanleggen van de lcd    
}

//Initialisatie voor al je infrarood sensoren
void irsensor_INIT(){
  pinMode(cent10.irPIN, INPUT);
  pinMode(cent5.irPIN, INPUT);
	pinMode(cent20.irPIN, INPUT);
  pinMode(cent50.irPIN, INPUT);
  pinMode(eur1.irPIN, INPUT);
  pinMode(eur2.irPIN, INPUT);
}

//Initialisatie voor al je drukknoppen
void button_INIT(){
  pinMode(cent5.btnPIN, INPUT);
  pinMode(cent10.btnPIN, INPUT);
  pinMode(cent20.btnPIN, INPUT);
  pinMode(cent50.btnPIN, INPUT);
  pinMode(eur1.btnPIN, INPUT);
  pinMode(eur2.btnPIN, INPUT);
}

//Initialisatie voor je interrupt
void interrupt_INIT(){
  //Enkel van hoog naar laag zal de interrupt activeren
	attachInterrupt(cent10.irPIN, isr10c, FALLING); //Deze interrupts aan de pinnen hangen
  attachInterrupt(cent5.irPIN, isr5c, FALLING);
  attachInterrupt(cent20.irPIN, isr20c, FALLING);
  attachInterrupt(cent50.irPIN, isr50c, FALLING);
  attachInterrupt(eur1.irPIN, isr1eur, FALLING);
  attachInterrupt(eur2.irPIN, isr2eur, FALLING);
}

//Initialisatie voor je schuifregister
void schuifregister_INIT(){
  // de 74HC595 pinnen als uitgangen instellen
  pinMode(latchPin, OUTPUT); //ST_CP pin van de 74HC595
  pinMode(clkPin, OUTPUT); //SH_CP pin van de 74HC595
  pinMode(dtPin, OUTPUT); //DS pin van de 74HC595
}

//Initialistaie voor je servo
void servo_INIT(){
	ESP32PWM::allocateTimer(2); //Timer allocaten voor de servo
	myservo.setPeriodHertz(50); //Standaard 50 hz servo gebruiken
	myservo.attach(servoPin, 500, 2400); //Servo verbinden met de servopin
}

//Initialisatie voor je bluetooth low energy
void ble_INIT(){
  // Create the BLE Device
  BLEDevice::init("Munt-sorteer systeem");

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
  eersteKeerVerbonden(); //Pollen wanneer je net bent verbonden met bluetooth
  disconnecting(); //Zorgen dat je ook kunt disconnecten
  lcdUpdaten(); //Lcd updaten met het totale geldbedrag
  drukknoppenLezen(); //Drukknoppen inlezen
  maximaalAantalGeld(); //Indien er meer geld inzet uitwerpen
  geldBedragPollen(); //Pollen voor als het geldbedrag is verandert om met interrupts te werken
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
    lcd.setCursor(0, 1);
    lcd.write(0); //Speciaal euro teken
    bedragVerandert = false; 
  }
  if(customLcdModus && customLcdTekstUpdaten){ //De LCD updaten met eigen tekst
    updateLCD(customLcdTekst, " ");
    customLcdTekstUpdaten = false;
  }
  if(deviceConnected && !customLcdModus){ //Enkel als het systeem verbonden is bluetooth logo tonen
    //Alle aparte stukken tekenen om het bluetooth logo te bekomen
    lcd.setCursor(14, 0); 
    lcd.write(1);
    lcd.setCursor(15, 0);
    lcd.write(3);
    lcd.setCursor(14, 1);
    lcd.write(2);
    lcd.setCursor(15, 1);
    lcd.write(3);
  }
  else if (!customLcdModus){
    //Anders hier niets tekenen zodat het logo weg is
    lcd.setCursor(14, 0);
    lcd.print(" ");
    lcd.setCursor(15, 0);
    lcd.print(" ");
    lcd.setCursor(14, 1);
    lcd.print(" ");
    lcd.setCursor(15, 1);
    lcd.print(" ");
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
      delay(800);
      startWaardenVersturen(); //Alle waarden doorsturen
      boot = false;
     }
  }
  else{
    boot = true; //Als het niet verbonden is is boot terug true
  }
}

//Alle drukknoppen inlezen
void drukknoppenLezen(){
  if((digitalRead(cent5.btnPIN) && !btn5c) && knoppenWerken){ 
    btn5c = true; //Zorgen dat je maar 1x de functie uitvoert als je erop klikt
  }
  else if((!digitalRead(cent5.btnPIN) && btn5c) && knoppenWerken){
    dispenseCoin(5);
    btn5c = false;
  }
  if ((digitalRead(cent10.btnPIN) && !btn10c) && knoppenWerken){
    btn10c = true;
  }else if((!digitalRead(cent10.btnPIN) && btn10c) && knoppenWerken){
    dispenseCoin(10);
    btn10c = false;
  }
  if ((digitalRead(cent20.btnPIN) && !btn20c) && knoppenWerken){
    btn20c = true;
  }else if((!digitalRead(cent20.btnPIN) && btn20c) && knoppenWerken){
    dispenseCoin(20);
    btn20c= false;
  }
  if ((digitalRead(cent50.btnPIN) && !btn50c) && knoppenWerken){
    btn50c= true;
  }else if((!digitalRead(cent50.btnPIN) && btn50c) && knoppenWerken){
    dispenseCoin(50);
    btn50c= false;
  }
  if ((digitalRead(eur1.btnPIN) && !btn1eur) && knoppenWerken){
    btn1eur = true;
  }else if((!digitalRead(eur1.btnPIN) && btn1eur) && knoppenWerken){
    dispenseCoin(100);
    btn1eur = false;
  }
  if ((digitalRead(eur2.btnPIN) && !btn2eur) && knoppenWerken){
    btn2eur = true;
  }else if((!digitalRead(eur2.btnPIN) && btn2eur) && knoppenWerken){
    dispenseCoin(200);
    btn2eur = false;
  }
}

//Enkel tot het maximaal geldaantal toelaten
void maximaalAantalGeld(){
  //Als het maximaal geldaantallen overschreden word zal het terug uitgeworpen worden
  if(cent5.aantal > cent5.maxAantal){
    delay(200); //Even wachten zodat de servo volledig kan draaien
    dispenseCoin(5); //geld uitwerpen
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
    EEPROM.writeUInt(cent5.adresAantal, cent5.aantal); //deze nieuwe waarde naar de EEPROM sturen
    String verzendString = "o5c" + String(cent5.aantal); 
    dataVersturen(verzendString); //Ook naar het verbonden apparaat sturen
    cent5changed = false;
    if(nietInlatenModus){ //Wanner er niets in het systeem mag dit terug uitwerpen
      dispenseCoin(5);
    }
  }
   if(cent10changed){
    EEPROM.writeUInt(cent10.adresAantal, cent10.aantal);
    String verzendString = "o10c" + String(cent10.aantal);
    dataVersturen(verzendString);
    cent10changed = false;
    if(nietInlatenModus){
      dispenseCoin(10);
    }
  }
   if(cent20changed){
    EEPROM.writeUInt(cent20.adresAantal, cent20.aantal);
    String verzendString = "o20c" + String(cent20.aantal);
    dataVersturen(verzendString);
    cent20changed = false;
    if(nietInlatenModus){
      dispenseCoin(20);
    }
  }
   if(cent50changed){
    EEPROM.writeUInt(cent50.adresAantal, cent50.aantal);
    String verzendString = "o50c" + String(cent50.aantal);
    dataVersturen(verzendString);
    cent50changed = false;
    if(nietInlatenModus){
      dispenseCoin(50);
    }
  }
   if(eur1changed){
    EEPROM.writeUInt(eur1.adresAantal, eur1.aantal);
    String verzendString = "o100c" + String(eur1.aantal);
    dataVersturen(verzendString);
    eur1changed = false;
    if(nietInlatenModus){
      dispenseCoin(100);
    }
  }
   if(eur2changed){
    EEPROM.writeUInt(eur2.adresAantal, eur2.aantal);
    String verzendString = "o200c" + String(eur2.aantal);
    dataVersturen(verzendString);
    eur2changed = false;
    if(nietInlatenModus){
      dispenseCoin(200);
    }
  }
  EEPROM.commit(); //Alles dan zeker opslaan in de EEPROM
}

//Functie om het totale geldbedrag te berekenen
float totaleBedrag(){
  return (eur2.aantal * 2 + eur1.aantal + cent50.aantal * 0.5 + cent20.aantal * 0.2 + cent10.aantal * 0.1 + cent5.aantal * 0.05);
}

//Als het systeem net verbonden is zal hij alle startwaarden doorsturen
void startWaardenVersturen(){
  //Alle aantallen van opgeslagen munten sturen
  dataVersturen("o200c" + String(eur2.aantal));
  dataVersturen("o100c" + String(eur1.aantal));
  dataVersturen("o50c" + String(cent50.aantal));
  dataVersturen("o20c" + String(cent20.aantal));
  dataVersturen("o10c" + String(cent10.aantal));
  dataVersturen("o5c" + String(cent5.aantal));
  //Alle maximale waarden van de munten sturen
  dataVersturen("om200c" + String(eur2.maxAantal));
  dataVersturen("om100c" + String(eur1.maxAantal));
  dataVersturen("om50c" + String(cent50.maxAantal));
  dataVersturen("om20c" + String(cent20.maxAantal));
  dataVersturen("om10c" + String(cent10.maxAantal));
  dataVersturen("om5c" + String(cent5.maxAantal));
}

//LCD updaten met de parameters die je hier kan meegeven
void updateLCD(String firstline, String secondLine){
  lcd.clear(); //Eerst alles verwijderen van het scherm
  lcd.setCursor(0, 0); //Eerste lijn
  lcd.print(firstline);
  lcd.setCursor(1, 1); //Tweeded lijn, 1 naar rechts opschuiven voor het euro teken
  lcd.print(secondLine);
}

//De verzonden data verwerken
void dataVerwerken(String verzondenData){
  Serial.println(verzondenData);
  char firstLetter = verzondenData.charAt(0); //Eerste karakter inlezen
  if(firstLetter == 's'){ //Geldtype uitwerpen
    String derestVanDeString = verzondenData.substring(1); //Het werkelijke geldtype uitlezen
    //Dan kijken welke waarde ze willen uitwerpen
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
  else if(firstLetter == 'l'){ //LCD
    char secondLetter = verzondenData.charAt(1); //Wat met de lcd doen
    if(secondLetter == 's'){ //Tekst op de lcd schrijven
      String tekst = verzondenData.substring(2); //De rest van de string op de lcd schrijven
      customLcdTekst = tekst;
      customLcdTekstUpdaten = true;
    }
    else if(secondLetter == 'm'){ //Modus van de lcd wijzigen
      String derestVanDeString = verzondenData.substring(2); //Speciale modus aan of af
      if(derestVanDeString == "1"){
        customLcdModus = true;
      }
      else{
        customLcdModus = false;
      }
    }
    else{
      String derestVanDeString = verzondenData.substring(1); //Lcd af of aan
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
      String geldType = verzondenData.substring(2, verzondenData.lastIndexOf('c')); //Eerst het type van het geld bekijken
      int maxGeldAantal = verzondenData.substring(verzondenData.lastIndexOf('c') + 1).toInt(); //Vervolgens het aantal van dit geldtype omzetten
      if(maxGeldAantal > 0){ //Enkel een maximale waarde accepteren als deze groter is dan 0
        if (geldType == "5") {
            cent5.maxAantal = maxGeldAantal; //Deze waarde dan ook veranderen
            dataVersturen("om5c" + String(cent5.maxAantal)); //De veranderde waarde terug doorsturen
        }else if (geldType == "10") {
            cent10.maxAantal = maxGeldAantal;
            dataVersturen("om10c" + String(cent10.maxAantal));
        } else if (geldType == "20") {
            cent20.maxAantal = maxGeldAantal;
            dataVersturen("om20c" + String(cent20.maxAantal));
        } else if (geldType == "50") {
            cent50.maxAantal = maxGeldAantal;
            dataVersturen("om50c" + String(cent50.maxAantal));
        } else if (geldType == "100") {
            eur1.maxAantal = maxGeldAantal;
            dataVersturen("om100c" + String(eur1.maxAantal));
        } else if (geldType == "200") {
            eur2.maxAantal = maxGeldAantal;
            dataVersturen("om200c" + String(eur2.maxAantal));
        }
    }
    }
    else{ //Anders is dit een nieuwe geldwaarde
    String geldType = verzondenData.substring(1, verzondenData.lastIndexOf('c')); // Eerst het type van het geld bekijken
    int geldAantal = verzondenData.substring(verzondenData.lastIndexOf('c') + 1).toInt(); // Vervolgens het aantal van dit geldtype omzetten
    if(geldAantal >= 0){ //Enkel toelaten als dit gelijk aan of groter dan 0 is
    if (geldType == "5") {
        cent5.aantal = geldAantal; //De geldwaarde veranderen
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
    }
    bedragVerandert = true; //Het totale bedrag is verandert
  }
  else if(firstLetter == 'u'){ //Een zelfgekozen geldbedrag uitwerpen
    int geldAantal = verzondenData.substring(1).toInt(); //Het bedrag dat uitgeworpen moet worden
    geld_Aantal_Uitwerpen(geldAantal); //Dit dan uitwerpen
  }
  else if(firstLetter == 'd'){ //Doorlaatmodus
    String derestVanDeString = verzondenData.substring(1); //Wel of niet doorlaten
    if(derestVanDeString == "1"){
      nietInlatenModus = true;
    }
    else{
      nietInlatenModus = false;
    }
  }
  else if(firstLetter == 'k'){ //Knoppen in of uitschakelen
    String derestVanDeString = verzondenData.substring(1);
    if(derestVanDeString == "1"){
      knoppenWerken = true;
    }
    else{
      knoppenWerken = false;
    }
  }
}

//Het servosignaal om 1 geldstukje uit te werpen
void signaalServo(){
  myservo.write(180); //Servo 180 graden draaien
  delay(400); //Zorgen dat er genoeg tijd is voor de servo om volledig te draaien
  myservo.write(0); //Servo terug naar 0 graden draaien
}

//Een geldtype uitwerpen
void dispenseCoin(int amount){
  switch(amount){
    case 5: //5 cent
      if(cent5.aantal > 0){ //Enkel uitvoeren als je geld van deze soort hebt
        updateShiftRegister(0b00001000); //Via het schuifregister selecteren welke servo je met aansturen
        cent5.aantal--; 
        cent5changed = true;
        signaalServo(); //Signaal versturen
      }
      break;
    case 10: //10 cent
      if(cent10.aantal > 0){
      updateShiftRegister(0b00000100);
      cent10.aantal--;
      cent10changed = true;
      signaalServo();
      }
      break;
    case 20: //20 cent
      if(cent20.aantal > 0){
        updateShiftRegister(0b00010000);
        cent20.aantal--;
        cent20changed = true;
        signaalServo();
      }
      break;
    case 50: //50 cent
      if(cent50.aantal > 0){
        updateShiftRegister(0b01000000);
        cent50.aantal--;
        cent50changed = true;
        signaalServo();
      }
      break;
    case 100: // 1 euro
    if(eur1.aantal > 0){
        updateShiftRegister(0b00100000);
        eur1.aantal--;
        eur1changed = true;
        signaalServo();
      }
      break;
    case 200: // 2 euro
      if(eur2.aantal > 0){
        updateShiftRegister(0b10000000);
        eur2.aantal--;
        eur2changed = true;
        signaalServo();
      }
      break;
  }
  bedragVerandert = true; //De lcd updaten met de nieuwe waarde
}

//Een specifiek geldbedrag uitwerpen
void geld_Aantal_Uitwerpen(int bedrag){
	// Bereken de aantallen munten
	while (bedrag >= 200 && eur2.aantal > 0) { //Is het bedrag groter dan 2 euro en hebben we dit?
		dispenseCoin(200); //Indien ja dit uitwerpen
		bedrag -= 200;
    delay(400); //Wachten tot het volledig is uitgeworpen
	}
	while (bedrag >= 100 && eur1.aantal > 0) {
		dispenseCoin(100);
		bedrag -= 100;
    delay(400);
	}
	while (bedrag >= 50 && cent50.aantal > 0) {
		dispenseCoin(50);
		bedrag -= 50;
    delay(400);
	}
	while (bedrag >= 20 && cent20.aantal > 0) {
		dispenseCoin(20);
		bedrag -= 20;
    delay(400);
	}
	while (bedrag >= 10 && cent10.aantal > 0) {
		dispenseCoin(10);
		bedrag -= 10;
    delay(400);
	}
	while (bedrag >= 5 && cent5.aantal > 0) {
		dispenseCoin(5);
		bedrag -= 5;
    delay(400);
	}
}

//Deze functie is voor het aansturen van het schuifregister
void updateShiftRegister(int uitgangen)
{
  digitalWrite(latchPin, LOW); //Nog niet doorsturen
  shiftOut(dtPin, clkPin, LSBFIRST, uitgangen); //Data in het register schuiven 
  digitalWrite(latchPin, HIGH); //Doorsturen
}

//Data versturen via bluetooth low energy
void dataVersturen(String toSend){
    std::string stdToSend = toSend.c_str(); // Convert String to std::string
    pTxCharacteristic->setValue(stdToSend);
    pTxCharacteristic->notify();
    delay(10); // Add delay to avoid congestion
}
