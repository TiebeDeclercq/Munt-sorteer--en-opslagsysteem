/*
 * CoinCounterAtmega328p.c
 *
 * Created: 28/11/2023 20:46:50
 * Author : DeclercqTiebe
 */ 

#include <avr/io.h>
#define  F_CPU 16000000UL
#include <util/delay.h>

int main(void)
{
	DDRB |= (1<<PB4);
    /* Replace with your application code */
    while (1) 
    {
    }
}

