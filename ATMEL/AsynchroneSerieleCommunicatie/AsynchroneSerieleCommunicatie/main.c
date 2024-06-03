/*
 * AsynchroneSerieleCommunicatie.c
 *
 * Created: 20/10/2023 22:27:20
 * Author : DeclercqTiebe
 */ 

#include <avr/io.h>
#define F_CPU 3686400
#include <util/delay.h>
#include <avr/interrupt.h>

int main(void)
{

	DDRC = 0xFF;
	DDRA = 0;
	USART_Init();
	USART_Transmit_String("Verbonden");

    /* Replace with your application code */
    while (1) 
    {
		 if(PINA&(1<<PINA0)){
			 while(PINA&(1<<PINA0));
			 USART_Transmit_String("G165");
		 }
		if(PINA&(1<<PINA1)){
			while(PINA&(1<<PINA1));
			USART_Transmit_String("G235");
		}
		if(PINA&(1<<PINA2)){
			while(PINA&(1<<PINA2));
			USART_Transmit_String("v");
		}
		if(PINA&(1<<PINA3)){
			while(PINA&(1<<PINA3));
			USART_Transmit_String("L0");
		}
		if(PINA&(1<<PINA4)){
			while(PINA&(1<<PINA4));
			USART_Transmit_String("L1");
		}
		if(PINA&(1<<PINA5)){
			while(PINA&(1<<PINA5));
			USART_Transmit_String("O10c11");
		}
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



ISR(USART0_RX_vect) {
	unsigned char receivedData = UDR0; // Read the received data
	if(receivedData == 'V'){
		PORTC |= (1<<PORTC5);
		USART_Transmit_String("LED5");
	}
	else if(receivedData == 'R'){
		PORTC |= (1<<PORTC3);
		USART_Transmit_String("LED3");
	}
	
	// Process the received data here
}
