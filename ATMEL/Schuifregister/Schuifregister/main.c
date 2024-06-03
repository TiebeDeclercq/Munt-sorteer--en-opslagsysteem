/*
 * Schuifregister.c
 *
 * Created: 15/10/2023 20:46:48
 * Author : DeclercqTiebe
 */ 

#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 3686400			/* Define CPU Frequency e.g. here 8MHz */
#define HC595_PORT   PORTC
#define HC595_DDR    DDRC

#define HC595_DS_POS PC2      //Data pin (DS) pin location
#define HC595_SH_CP_POS PC3      //Shift Clock (SH_CP) pin location
#define HC595_ST_CP_POS PC4      //Store Clock (ST_CP) pin location
void shiftInit()
{
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
/*
Main High level function to write a single byte to
Output shift register 74HC595.
Arguments:
   single byte to write to the 74HC595 IC
Returns:
   NONE
Description:
   The byte is serially transfered to 74HC595
   and then latched. The byte is then available on
   output line Q0 to Q7 of the HC595 IC.
*/
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

void main()
{
	//Configure TIMER1

	TCCR1A|=((1<<COM1A1)|(1<<COM1B1)|(1<<WGM11)); //NON Inverted PWM

	TCCR1B|=((1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10)); //PRESCALER=1 MODE 14(FAST PWM)
	
	ICR1=1151; //fPWM=50Hz (Period = 20ms Standard).

	DDRD|=((1<<DDRD4)|(1<<DDRD5)); //PWM Pins as Out
	
	DDRA &= ((1<<DDRA0)|(1<<DDRA1)|(1<<DDRA2));
	
   uint8_t led[8]={
                        0b10000000,
                        0b01000000,
						0b00100000,
						0b00010000,
						0b00001000,
						0b00000100,
						0b00000010,
						0b00000001,
                     };
   shiftInit(); //Initialise
   while(1)
   {
      /*for(uint8_t i=0;i<8;i++)
      {
         shiftWrite(led[i]);   //Write the data to shift register
         _delay_ms(400);             //Wait
      }*/
	  shiftWrite(0b00000001);   //Write the data to shift register
	  if(PINA&(1<<PINA0)){
		  while(PINA&(1<<PINA0));
		  _delay_ms(15);//dender knop
		  OCR1A=140; //180 graden
		  _delay_ms(300);
		  OCR1A= 25; //0 graden
		  _delay_ms(500);
	  }
   }
}

