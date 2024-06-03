/*
 * CoinCounter.c
 *
 * Created: 20/10/2023 23:35:09
 * Author : DeclercqTiebe
 */ 

#include <avr/io.h>
#define F_CPU 3686400
#include <util/delay.h>
#include <avr/interrupt.h>

#include "liquid_crystal_i2c.h"

#define HC595_PORT   PORTC
#define HC595_DDR    DDRC

#define HC595_DS_POS PC2      //Data pin (DS) pin location
#define HC595_SH_CP_POS PC3      //Shift Clock (SH_CP) pin location
#define HC595_ST_CP_POS PC4      //Store Clock (ST_CP) pin location

// Muntenwaarden in centen
#define CENT_1 1
#define CENT_2 2
#define CENT_5 5
#define CENT_10 10
#define CENT_20 20
#define CENT_50 50
#define EURO_1 100
#define EURO_2 200

// Opslagplaatsen en het geld
int aantal5cent = 0;
int aantal10cent = 0;
int aantal20cent = 0;
int aantal50cent = 0;
int aantal1eur = 0;
int aantal2eur = 0;

uint8_t servos_Aantesturen = 0b00000000;
uint8_t servo_Actief = 0b00000000;

int coinsInt = 0;
char coins[2]; // Assuming a maximum of 2 characters in the resulting string

char rx_buff[20]; // Definieer een char-array met een maximale lengte van 20 tekens



int main(void)
{
	LiquidCrystalDevice_t device = lq_init(0x27, 16, 2, LCD_5x8DOTS); //Initialiseer display
	
    DDRA &= ((1<<DDRA0)|(1<<DDRA1)|(1<<DDRA2)|(1<<DDRA3)|(1<<DDRA4)|(1<<DDRA5)); //INGANG DRUKKNOPPEN
    DDRD |= (1<<DDRD5);
	servo_Init();
	shift_Init();
	USART_Init();
	
/*	
	lq_turnOnBacklight(&device); // simply turning on the backlight
	lq_print(&device, "GIP");
	lq_setCursor(&device, 1, 0); // moving cursor to the next line
	lq_print(&device, "Tiebe Declercq");
*/
    while (1) 
    {
		geld_Uitwerpen(5);
		geld_Uitwerpen(10);
		geld_Uitwerpen(20);
		geld_Uitwerpen(50);
		geld_Uitwerpen(1);
		geld_Uitwerpen(2);
	

		
	//geld_Detectie();
	//uitwerpen_knop();
    }
	
}
void geld_Detectie(){
	
 static unsigned int detectOnceFlag5c = 0;
 static unsigned int detectOnceFlag10c = 0;
 static unsigned int detectOnceFlag20c = 0;
 static unsigned int detectOnceFlag50c = 0;
 static unsigned int detectOnceFlag1eur = 0;
 static unsigned int detectOnceFlag2eur = 0;
 	
char cDoorsturen[8];

if(detectOnceFlag10c == 0 && (PINA&(1<<PINA0))){
	detectOnceFlag10c = 1;
}
else if(detectOnceFlag10c == 1 && !(PINA&(1<<PINA0))){
	detectOnceFlag10c = 0;
		aantal10cent++;
		sprintf(cDoorsturen, "O10c%d", aantal10cent); //%d voor het aantal
		USART_Transmit_String(cDoorsturen);
}

if(detectOnceFlag5c == 0 && (PINA&(1<<PINA1))){
	detectOnceFlag5c = 1;
}
else if(detectOnceFlag5c == 1 && !(PINA&(1<<PINA1))){
	detectOnceFlag5c = 0;
	aantal5cent++;
	sprintf(cDoorsturen, "O5c%d", aantal5cent);
	USART_Transmit_String(cDoorsturen);
}

if(detectOnceFlag20c == 0 && (PINA&(1<<PINA2))){
	detectOnceFlag20c = 1;
}
else if(detectOnceFlag20c == 1 && !(PINA&(1<<PINA2))){
	detectOnceFlag20c = 0;
	aantal20cent++;
	sprintf(cDoorsturen, "O20c%d", aantal20cent);
	USART_Transmit_String(cDoorsturen);
}

if(detectOnceFlag1eur == 0 && (PINA&(1<<PINA3))){
	detectOnceFlag1eur = 1;
}
else if(detectOnceFlag1eur == 1 && !(PINA&(1<<PINA3))){
	detectOnceFlag1eur = 0;
	aantal1eur++;
	sprintf(cDoorsturen, "O100c%d", aantal1eur);
	USART_Transmit_String(cDoorsturen);
}

if(detectOnceFlag50c == 0 && (PINA&(1<<PINA4))){
	detectOnceFlag50c = 1;
}
else if(detectOnceFlag50c == 1 && !(PINA&(1<<PINA4))){
	detectOnceFlag50c = 0;
	aantal50cent++;
	sprintf(cDoorsturen, "O50c%d", aantal50cent);
	USART_Transmit_String(cDoorsturen);
}

if(detectOnceFlag2eur == 0 && (PINA&(1<<PINA5))){
	detectOnceFlag2eur = 1;
}
else if(detectOnceFlag2eur == 1 && !(PINA&(1<<PINA5))){
	detectOnceFlag2eur = 0;
	aantal2eur++;
	sprintf(cDoorsturen, "O200c%d", aantal2eur);
	USART_Transmit_String(cDoorsturen);
}
}
void USART_Init(void){
	UBRR0 = 23; //BAUD RATE VAN 9600
	UCSR0C |= ((1<<UCSZ01)|(1<<UCSZ00)); //8 databits
	UCSR0B |= ((1<<RXEN0)|(1<<TXEN0)); //8 databits
	UCSR0B |= (1<<RXCIE0); //Interrupt bij ontvangen
	// Enable global interrupts
	sei();
}

void USART_Transmit_Char(unsigned char data){
	//Wacht tot de transmit buffer leeg is
	while(!(UCSR0A&(1<<UDRE0))); //Wachten tot UDRE0 == 1
	UDR0 = data;
}

void USART_Transmit_String(char s[]){
	int i = 0;
	while(i < 64){
		if(s[i] == '\0') break;
		USART_Transmit_Char(s[i++]);
	}
}

unsigned char USART_Receive(void){
	//Wacht tot er een karakter is ontvangen
	while(!(UCSR0A&(1<<RXC0))); //Wachten tot RXC0 == 1
	return(UDR0); //Inhoud van UDR0 als terugkeerwaarde
}

ISR(USART0_RX_vect) { //Interrupt bij hoog flank van de rx 
	static unsigned int pos = 1;
	rx_buff[pos] = UDR0;
	if(rx_buff[pos] == ' '){ //Einde van de data
		rx_buff[pos]='\n';
		pos = 0;
		dataVerwerken();
	}
	else{
		pos++;
		}
}

void dataVerwerken(void){
	
	if(strstr(rx_buff, "s")) //Servo activeren S[nummer servo] = S5 = 5cent uitwerpen
	{
		char *numString = strstr(rx_buff, "s") + strlen("s");
		int num = atoi(numString);
		geld_Uitwerpen(num);
	}
	else if(strstr(rx_buff, "u")) //Geldbedrag uitwerpen U[156] = 1eur56cent
	{
		char *numString = strstr(rx_buff, "u") + strlen("u");
		int num = atoi(numString);
		geld_Aantal_Uitwerpen(num);
	}
	else if(strstr(rx_buff, "b")) //Buzzer als het 0 is af en 1 aan B[0]
	{
		char *numString = strstr(rx_buff, "b") + strlen("b");
		int num = atoi(numString);
		if(num == 1){
			//Bool buzzer op 1 zetten 
			USART_Transmit_String("Buzzer aan");
		}
		else{
			//Bool buzzer op 0 zetten
			USART_Transmit_String("Buzzer uit");
		}
	}
	else if(strstr(rx_buff, "v")) //C# programma controleerd of er verbinding is
	{
		USART_Transmit_String("v");
	}
}

void geld_Aantal_Uitwerpen(int bedrag){
	 // Variabelen om de aantallen van elke munt bij te houden
	 int euro2Count = 0;
	 int euro1Count = 0;
	 int cent50Count = 0;
	 int cent20Count = 0;
	 int cent10Count = 0;
	 int cent5Count = 0;

	 // Bereken de aantallen munten
	 while (bedrag >= EURO_2) {
		 euro2Count++;
		 bedrag -= EURO_2;
	 }
	 while (bedrag >= EURO_1) {
		 euro1Count++;
		 bedrag -= EURO_1;
	 }
	 while (bedrag >= CENT_50) {
		 cent50Count++;
		 bedrag -= CENT_50;
	 }
	 while (bedrag >= CENT_20) {
		 cent20Count++;
		 bedrag -= CENT_20;
	 }
	 while (bedrag >= CENT_10) {
		 cent10Count++;
		 bedrag -= CENT_10;
	 }
	 while (bedrag >= CENT_5) {
		 cent5Count++;
		 bedrag -= CENT_5;
	 }
	 
}
void updateLCD(LiquidCrystalDevice_t lcd){
	lq_clear(&lcd);
	sprintf(coins, "%d", coinsInt); // Convert int to string
	lq_print(&lcd, coins);
}
//Elke knop werpt 1 type kleingeld uit
void uitwerpen_knop(void){
	if(PINA&(1<<PINA0)){
		while(PINA&(1<<PINA0));
		_delay_ms(15);
		geld_Uitwerpen(10);
	}
	if(PINA&(1<<PINA1)){
		while(PINA&(1<<PINA1));
		_delay_ms(15);
		geld_Uitwerpen(5);
	}
	if(PINA&(1<<PINA2)){
		while(PINA&(1<<PINA2));
		_delay_ms(15);
		geld_Uitwerpen(20);
	}
	if(PINA&(1<<PINA3)){
		while(PINA&(1<<PINA3));
		_delay_ms(15);
		geld_Uitwerpen(1);
	}
	if(PINA&(1<<PINA4)){
		while(PINA&(1<<PINA4));
		_delay_ms(15);
		geld_Uitwerpen(50);
	}
	if(PINA&(1<<PINA5)){
		while(PINA&(1<<PINA5));
		_delay_ms(15);
		geld_Uitwerpen(2);
	}
}

//Het uitwerpen van geld
void geld_Uitwerpen(int geldBedrag){
	switch(geldBedrag){
		case 10:
			shiftWrite(0b00000001);
			break;
		case 5:
			shiftWrite(0b00000010);
			break;
		case 20:
			shiftWrite(0b00000100);
			break;
		case 1:
			shiftWrite(0b00001000);
			break;
		case 50:
			shiftWrite(0b00010000);
			break;
		case 2:
			shiftWrite(0b00100000);
			break;
	}
	OCR1A=140; //180 graden
	_delay_ms(300); //Genoeg tijd om volledig uit te komen geven
	OCR1A= 25; //0 graden
	_delay_ms(500); //Genoeg tijd om terug te keren geven
	shiftWrite(0b00000000);
}

//HET INITIALISEREN VAN DE LCD IN I2C
void lcd_Init(LiquidCrystalDevice_t lcd){
	lq_turnOnBacklight(&lcd); // simply turning on the backlight
	lq_print(&lcd, "GIP");
	lq_setCursor(&lcd, 1, 0); // moving cursor to the next line
	lq_print(&lcd, "Tiebe Declercq");
}

//HET INITIALISEREN VAN DE SERVO
void servo_Init(void){
	TCCR1A|=((1<<COM1A1)|(1<<COM1B1)|(1<<WGM11)); //NON Inverted PWM
	TCCR1B|=((1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10)); //PRESCALER=1 MODE 14(FAST PWM)
	ICR1=1151; //20ms periode
	DDRD|=((1<<DDRD4)|(1<<DDRD5)); //PWM Pins as Out
}

void shift_Init(void){
	 //Make the Data(DS), Shift clock (SH_CP), Store Clock (ST_CP) lines output
	 HC595_DDR|=((1<<HC595_SH_CP_POS)|(1<<HC595_ST_CP_POS)|(1<<HC595_DS_POS));
}

// change data (DS)lines
#define HC595DataHigh() (HC595_PORT|=(1<<HC595_DS_POS))
#define HC595DataLow() (HC595_PORT&=(~(1<<HC595_DS_POS)))

//Sends a clock pulse on SH_CP line
void shiftPulse()
{
	//Pulse the Shift Clock
	HC595_PORT|=(1<<HC595_SH_CP_POS);//HIGH
	HC595_PORT&=(~(1<<HC595_SH_CP_POS));//LOW
}

//Sends a clock pulse on ST_CP line
void shiftLatch()
{
	//Pulse the Store Clock
	HC595_PORT|=(1<<HC595_ST_CP_POS);//HIGH
	_delay_loop_1(1);
	HC595_PORT&=(~(1<<HC595_ST_CP_POS));//LOW
	_delay_loop_1(1);
}

void shiftWrite(uint8_t data)
{
	//Send each 8 bits serially
	//Order is MSB first
	for(uint8_t i=0;i<8;i++)
	{
		//Output the data on DS line according to the
		//Value of MSB
		if(data & 0b10000000)
		{
			//MSB is 1 so output high
			HC595DataHigh();
		}
		else
		{
			//MSB is 0 so output high
			HC595DataLow();
		}
		shiftPulse();  //Pulse the Clock line
		data=data<<1;  //Now bring next bit at MSB position
	}
	//Now all 8 bits have been transferred to shift register
	//Move them to output latch at one
	shiftLatch();
}
