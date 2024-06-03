/*
 * 74HC165.c
 *
 * Created: 29/11/2023 19:20:18
 * Author : Tiebe Declercq
 */ 

#include <avr/io.h>
#define F_CPU 3686400			/* Define CPU Frequency e.g. here 8MHz */
#include <util/delay.h>

int main(void)
{

DDRC |= ((1<<DDRC1)|(1<<DDRC2)|(1<<DDRC5));
DDRD &= ~(1<<DDRD1);

PORTC &= ~(1<<PORTC5);

DDRA = 0;
load();

while (1)
{


if(PIND&(1<<PIND1)){
PORTC |= (1<<PORTC5);
}
else{
PORTC &= ~(1<<PORTC5);
}
if(PINA&(1<<PINA0)){
	while(PINA&(1<<PINA0));
	_delay_ms(15);
	clockPuls();
}

if(PINA&(1<<PINA1)){
	while(PINA&(1<<PINA0));
	_delay_ms(15);

}

}
}

void clockPuls(){
	PORTC |= (1<<PORTC1);
	_delay_ms(15);
	PORTC &= ~(1<<PORTC5);
}
void load(){
	PORTC &= ~(1<<PORTC2);
	_delay_ms(15);
	PORTC |= (1<<PORTC2);
}
