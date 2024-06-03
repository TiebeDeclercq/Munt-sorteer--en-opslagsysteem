/*
 * Munt sorteer- en opslagsysteem.c
 *
 * Created: 19/11/2023 20:09:19
 * Author : Tiebe Declercq
 */ 

//ALGEMENE INCLUDES
#include <avr/io.h>
#include "liquid_crystal_i2c.h"
#define F_CPU 3686400
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//SCHUIFREGISTER INCLUDES
#define HC595_PORT   PORTC
#define HC595_DDR    DDRC
#define HC595_DS_POS PC2      //Data pin (DS) pin locatie
#define HC595_SH_CP_POS PC4      //Shift Clock (SH_CP) pin locatie
#define HC595_ST_CP_POS PC3      //Store Clock (ST_CP) pin locatie

// Muntenwaarden in centen
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

//COMMUNICATIE
char rx_buff[20]; // Definieer een char-array met een maximale lengte van 20 tekens

int detectOnceFlag5c = 0;
int detectOnceFlag10c = 0;
int detectOnceFlag20c = 0;
int detectOnceFlag50c = 0;
int detectOnceFlag1eur = 0;
int detectOnceFlag2eur = 0;

int sorteerModus = 0;
int buzzerAan = 0;


// Compiler laten weten van de functies
void USART_Init(void);
void servo_Init(void);
void shift_Init(void);
void geld_Detectie(void);
void geld_Uitwerpen(int);
void shiftWrite(uint8_t);
void USART_Transmit_String(char s[]);
void dataVerwerken(void);
void timer_Init(void);
void timerBuzzer_Start(void);
void timerBuzzer_Stop(void);
void buzzer(void);
void calculateTotalEuros(void);

LiquidCrystalDevice_t device;

int main(void)
{
	DDRA = 0; //Volledig poort a zijn knoppen
	
	servo_Init();//Init servo
	shift_Init(); //Init schuifregister
	USART_Init(); //Init communicatie
	timer_Init();
	
	device = lq_init(0x27, 16, 2, LCD_5x8DOTS); // intialize 2-lines display
	//lq_turnOnBacklight(&device); // simply turning on the backlight
	lq_print(&device, "GIP");
	lq_setCursor(&device, 1, 0); // moving cursor to the next line	
	lq_print(&device, "Tiebe Declercq");
	
	sei();
	
	PCICR |= (1<<PCIE0);
	PCMSK0 |= ((1<<PCINT0)|(1<<PCINT1)|(1<<PCINT3)|(1<<PCINT4)|(1<<PCINT5)|(1<<PCINT6));
	
	DDRC |= (1<<DDRC5);
	
    while (1) 
    {
		
    }
}

void calculateTotalEuros() {
	int totalEuros = 0;

	totalEuros += aantal2eur * EURO_2;
	totalEuros += aantal1eur * EURO_1;
	totalEuros += aantal50cent * CENT_50;
	totalEuros += aantal20cent * CENT_20;
	totalEuros += aantal10cent * CENT_10;
	totalEuros += aantal5cent * CENT_5;
	
	// Format the result as a string with the Euro sign
	char resultString[20];  // Adjust the size based on your needs
}

//Het uitwerpen van een enkel muntstuk
void geld_Uitwerpen(int geldBedrag){
	switch(geldBedrag){
		case 10:
			shiftWrite(0b00000001); //Het schuifregister aansturen
			aantal10cent--; //Aantal 10 cent verminderen
			char final_string_10c[20]; 
			sprintf(final_string_10c, "O10c%d\n", aantal10cent);
			USART_Transmit_String(final_string_10c); //String versturen met het nieuwe geldaantal
		break;
		case 5:
			shiftWrite(0b00000010);
			aantal5cent--;
			char final_string_5c[20];
			sprintf(final_string_5c, "O5c%d\n", aantal5cent);
			USART_Transmit_String(final_string_5c);
		break;
		case 20:
			shiftWrite(0b00000100);
			aantal20cent--;
			char final_string_20c[20];
			sprintf(final_string_20c, "O20c%d\n", aantal20cent);
			USART_Transmit_String(final_string_20c);			
		break;
		case 1:
			shiftWrite(0b00001000);
			aantal1eur--;
			char final_string_1eur[20];
			sprintf(final_string_1eur, "O100c%d\n", aantal1eur);
			USART_Transmit_String(final_string_1eur);
		break;
		case 50:
			shiftWrite(0b00010000);
			aantal50cent--;
			char final_string_50c[20];
			sprintf(final_string_50c, "O50c%d\n", aantal50cent);
			USART_Transmit_String(final_string_50c);
		break;
		case 2:
			shiftWrite(0b00100000);
			aantal2eur--;
			char final_string_2eur[20];
			sprintf(final_string_2eur, "O200c%d\n", aantal2eur);
			USART_Transmit_String(final_string_2eur);
		break;
		case 385:
			shiftWrite(0b00111111);
		break;
	}
	buzzer();
	OCR1A=140; //180 graden
	_delay_ms(300); //Genoeg tijd om volledig uit te komen geven
	OCR1A= 25; //0 graden
	_delay_ms(500); //Genoeg tijd om terug te keren geven
	shiftWrite(0b00000000);
}

void buzzer(void){ //Laat de buzzer werken
	if(buzzerAan){ //Controleer of de gebruiker de buzzer wel wil gebruiken
		PORTC |= (1<<PORTC5);
		timerBuzzer_Start();
	}

}

void geld_Aantal_Uitwerpen(int bedrag){
	// Bereken de aantallen munten
	while (bedrag >= 200 && aantal2eur > 0) {
		geld_Uitwerpen(2);
		bedrag -= 200;
	}
	while (bedrag >= 100 && aantal1eur > 0) {
		geld_Uitwerpen(1);
		bedrag -= 100;
	}
	while (bedrag >= 50 && aantal50cent > 0) {
		geld_Uitwerpen(50);
		bedrag -= 50;
	}
	while (bedrag >= 20 && aantal20cent > 0) {
		geld_Uitwerpen(20);
		bedrag -= 20;
	}
	while (bedrag >= 10 && aantal10cent > 0) {
		geld_Uitwerpen(10);
		bedrag -= 10;
	}
	while (bedrag >= 5 && aantal5cent > 0) {
		geld_Uitwerpen(5);
		bedrag -= 5;
	}
	
}

//Timer initialiseren
void timer_Init(void){
	TCCR0A |= (1<<WGM01);
	TIMSK0 |= (1<<OCIE0A);
	OCR0A = 1800;
	TIFR0 |= (1<<OCF0A);
	TCCR0A |= (1<<COM0A0);
}

//Buzzer starten
void timerBuzzer_Start(void){
	TCCR0B |= (1<<CS00);
	TCCR0B |= (1<<CS02);
}

//Buzzer stoppen
void timerBuzzer_Stop(void){
	TCCR0B &= ~(1<<CS00);
	TCCR0B &= ~(1<<CS02);
}

//Initialisatie van de servo
void servo_Init(void){
	TCCR1A|=((1<<COM1A1)|(1<<COM1B1)|(1<<WGM11)); //NON Inverted PWM
	TCCR1B|=((1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10)); //PRESCALER=1 MODE 14(FAST PWM)
	ICR1=1151; //20ms periode
	DDRD|=((1<<DDRD4)|(1<<DDRD5)); //PWM Pins as Out
}

//Initialisatie van de communicatie
void USART_Init(void){
	UBRR0 = 23; //BAUD RATE VAN 9600
	UCSR0C |= ((1<<UCSZ01)|(1<<UCSZ00)); //8 databits
	UCSR0B |= ((1<<RXEN0)|(1<<TXEN0)); //8 databits
	UCSR0B |= (1<<RXCIE0); //Interrupt bij ontvangen
}

//Initialisatie van het schuifregister
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

//Een char serieel versturen
void USART_Transmit_Char(unsigned char data){
	//Wacht tot de transmit buffer leeg is
	while(!(UCSR0A&(1<<UDRE0))); //Wachten tot UDRE0 == 1
	UDR0 = data;
}

//Een string serieel versturen
void USART_Transmit_String(char s[]){
	int i = 0;
	while(i < 64){
		if(s[i] == '\0') break;
		USART_Transmit_Char(s[i++]);
	}
}

ISR(PCINT0_vect){
	if(buzzerAan){
		buzzer();
	}
	if(!(PINA&(1<<PINA0)) && detectOnceFlag2eur == 0){
		if(sorteerModus){ //Als je sorteert ook direct 1 terug uitwerpen
			_delay_ms(300);
			geld_Uitwerpen(2);
		}
		aantal2eur++;
		char final_string[20];
		sprintf(final_string, "O200c%d\n", aantal2eur);
		USART_Transmit_String(final_string);
		detectOnceFlag2eur = 1;
	}
	else if(PINA&(1<<PINA0)){
		detectOnceFlag2eur = 0;
	}
	
	if(!(PINA&(1<<PINA1)) && detectOnceFlag50c == 0){
		if(sorteerModus){
			_delay_ms(300);
			geld_Uitwerpen(50);
		}
		aantal50cent++;
		char final_string[20];
		sprintf(final_string, "O50c%d\n", aantal50cent);
		USART_Transmit_String(final_string);
		detectOnceFlag50c = 1;
	}
	else if(PINA&(1<<PINA1)){
		detectOnceFlag50c = 0;
	}
	
	if(!(PINA&(1<<PINA3)) && detectOnceFlag1eur == 0){
		if(sorteerModus){
			_delay_ms(300);
			geld_Uitwerpen(1);
		}
		aantal1eur++;
		char final_string[20];
		sprintf(final_string, "O100c%d\n", aantal1eur);
		USART_Transmit_String(final_string);
		detectOnceFlag1eur = 1;
	}
	else if(PINA&(1<<PINA3)){
		detectOnceFlag1eur = 0;
	}
	
	if(!(PINA&(1<<PINA4)) && detectOnceFlag20c == 0){
		if(sorteerModus){
			_delay_ms(300);
			geld_Uitwerpen(20);
		}
		aantal20cent++;
		char final_string[20];
		sprintf(final_string, "O20c%d\n", aantal20cent);
		USART_Transmit_String(final_string);
		detectOnceFlag20c = 1;
	}
	else if(PINA&(1<<PINA4)){
		detectOnceFlag20c = 0;
	}
	
	if(!(PINA&(1<<PINA5)) && detectOnceFlag5c == 0){
		if(sorteerModus){
			_delay_ms(300);
			geld_Uitwerpen(5);
		}
		aantal5cent++;
		char final_string[20];
		sprintf(final_string, "O5c%d\n", aantal5cent);
		USART_Transmit_String(final_string);
		detectOnceFlag5c = 1;
	}
	else if(PINA&(1<<PINA5)){
		detectOnceFlag5c = 0;
	}
	
		if(!(PINA&(1<<PINA6)) && detectOnceFlag10c == 0){
			if(sorteerModus){
				_delay_ms(300);
				geld_Uitwerpen(10 );
			}
			aantal10cent++;
			char final_string[20];
			sprintf(final_string, "O10c%d\n", aantal10cent);
			USART_Transmit_String(final_string);
			detectOnceFlag10c = 1;
		}
		else if(PINA&(1<<PINA6)){
			detectOnceFlag10c = 0;
		}
	_delay_ms(10);
	
	calculateTotalEuros();
}

ISR(TIMER0_COMPA_vect){
	PORTC &= ~(1<<PORTC5);
}

//Interrupt bij hoog niveau op rx
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

//De data verwerken
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
else if (strstr(rx_buff, "o")) {
	char *amountPtr = strstr(rx_buff, "c");

	if (amountPtr != NULL) {
		*amountPtr = '\0'; // Null-terminate the coin type string
		int amount = atoi(amountPtr + 1);

		// Move back to find the start of the coin type
		while (amountPtr > rx_buff && *(amountPtr - 1) != 'o') {
			amountPtr--;
		}

		// Now amountPtr points to the start of the coin type
		// Update variables based on the coin type
		if (strcmp(amountPtr, "10") == 0) {
			aantal10cent = amount;
			char final_string_wrong[20];
			sprintf(final_string_wrong, "O10c%d\n", aantal10cent);
			USART_Transmit_String(final_string_wrong);
			} 
			else if (strcmp(amountPtr, "5") == 0) {
			aantal5cent = amount;
			char final_string_wrong[20];
			sprintf(final_string_wrong, "O5c%d\n", aantal5cent);
			USART_Transmit_String(final_string_wrong);
			} 
			else if (strcmp(amountPtr, "20") == 0) {
			aantal20cent = amount;
			char final_string_wrong[20];
			sprintf(final_string_wrong, "O20c%d\n", aantal20cent);
			USART_Transmit_String(final_string_wrong);
			}
			else if (strcmp(amountPtr, "50") == 0) {
				aantal50cent = amount;
				char final_string_wrong[20];
				sprintf(final_string_wrong, "O50c%d\n", aantal50cent);
				USART_Transmit_String(final_string_wrong);
			}
			else if (strcmp(amountPtr, "100") == 0) {
				aantal1eur = amount;
				char final_string_wrong[20];
				sprintf(final_string_wrong, "O100c%d\n", aantal1eur);
				USART_Transmit_String(final_string_wrong);
			}
			else if (strcmp(amountPtr, "200") == 0) {
				aantal2eur = amount;
				char final_string_wrong[20];
				sprintf(final_string_wrong, "O200c%d\n", aantal2eur);
				USART_Transmit_String(final_string_wrong);
			}
	}
}
	else if(strstr(rx_buff, "b")) //Buzzer als het 0 is af en 1 aan B[0]
	{
		char *numString = strstr(rx_buff, "b") + strlen("b");
		int num = atoi(numString);
		if(num == 1){
			buzzerAan = 1;
		}
		else{
			buzzerAan = 0;
		}
	}
	else if(strstr(rx_buff, "d")) //doorlaten als het 0 is af en 1 aan d[0]
	{
		char *numString = strstr(rx_buff, "d") + strlen("d");
		int num = atoi(numString);
		if(num == 1){
			sorteerModus = 1;
		}
		else{
			sorteerModus = 0;
		}
	}
		else if(strstr(rx_buff, "l")) //lcd af als het 0 is en aan en 1 aan d[0]
		{
			char *numString = strstr(rx_buff, "l") + strlen("l");
			int num = atoi(numString);
			if(num == 1){
					lq_turnOnBacklight(&device); // simply turning on the backlight
					lq_turnOnDisplay(&device); // simply turning on the backlight
			}
			else{
					lq_turnOffBacklight(&device); // simply turning on the backlight
					lq_turnOffDisplay(&device); // simply turning on the backlight
			}
		}
	else if(strstr(rx_buff, "v")) //C# programma controleerd of er verbinding is
	{
		char final_string2eur[20];
		sprintf(final_string2eur, "O200c%d\n", aantal2eur);
		USART_Transmit_String(final_string2eur);
		_delay_ms(80);
		char final_string50c[20];
		sprintf(final_string50c, "O50c%d\n", aantal50cent);
		USART_Transmit_String(final_string50c);
		_delay_ms(80);
		char final_string1eur[20];
		sprintf(final_string1eur, "O100c%d\n", aantal1eur);
		USART_Transmit_String(final_string1eur);
		_delay_ms(80);
		char final_string20c[20];
		sprintf(final_string20c, "O20c%d\n", aantal20cent);
		USART_Transmit_String(final_string20c);
		_delay_ms(80);
		char final_string5c[20];
		sprintf(final_string5c, "O5c%d\n", aantal5cent);
		USART_Transmit_String(final_string5c);
		_delay_ms(80);
		char final_string10c[20];
		sprintf(final_string10c, "O10c%d\n", aantal10cent);
		USART_Transmit_String(final_string10c);
		
	}
}

void irSensor_init(){
	//GIMSK |= (1<<PCIE0);
}

