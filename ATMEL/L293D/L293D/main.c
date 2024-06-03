/*
 * L293D.c
 *
 * Created: 31/10/2023 20:59:06
 * Author : Tiebe Declercq
 */ 

#include <avr/io.h>
#define F_CPU 3686400
#include <util/delay.h>

int main(void)
{
	DDRC |= ((1<<DDRC0) | (1<<DDRC1));
	
	DDRB |= (1 << DDB3); //OCR0A als uitgang instellen
	
	DDRA &= ~((1<<DDRA0)|(1<<DDRA7)); //PA0, PA7 als ingang instellen
	
	OCR0A = 64; //25% duty cycle
	
	 TCCR0A |= (1 << COM0A1);  // Set OC0A op toggle on compare match
	  
	 TCCR0A |= ((1 << WGM01) | (1 << WGM00)); //Op fast PWM zetten
     TCCR0B &= ~(1<<WGM02);
	  
	 TCCR0B |= ((1 << CS00)|(1 << CS01)); //Prescaler van 64 gebruiken
	 TCCR0B &= ~(1<<CS02);

    while (1) 
    {
		PORTC |= (1<<PORTC0);
		PORTC &= ~(1<<PORTC1);
		
		
		if(PINA&(1<<PINA0) && OCR0A < 246){
			while(PINA&(1<<PINA0));
			_delay_ms(15);
			OCR0A += 26; //10% laten stijgen
		}
		if(PINA&(1<<PINA7) && OCR0A > 12){
			while(PINA&(1<<PINA7));
			_delay_ms(15);
			OCR0A -= 26; //10% laten dalen
		}
    }
}

