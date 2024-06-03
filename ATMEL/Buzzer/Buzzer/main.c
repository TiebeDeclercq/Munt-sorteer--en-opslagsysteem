/*
 * Buzzer.c
 *
 * Created: 31/10/2023 13:34:20
 * Author : Tiebe Declercq
 */ 

#include <avr/io.h>
#define F_CPU 3686400
#include <util/delay.h>

int main(void)
{
	DDRC |= (1<<DDRC0);
	
    /* Replace with your application code */
    while (1) 
    {
		PORTC |= (1<<PORTC0);

    }
}

