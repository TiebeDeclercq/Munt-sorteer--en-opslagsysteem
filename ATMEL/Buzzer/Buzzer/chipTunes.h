/*___________________________________________________________________________________________________

Title:
	chipTunes.h v2.0

Description:
	Play chip tunes and tones on a piezo buzzer using PWM. Made for AVR microcontrollers.
	
	For complete details visit
	https://www.programming-electronics-diy.xyz/2021/02/playing-music-and-tones-using-piezo.html

Author:
 	Liviu Istrate
	istrateliviu24@yahoo.com

Donate:
	Software development takes time and effort so if you find this useful consider a small donation at:
	paypal.me/alientransducer
_____________________________________________________________________________________________________*/


/* ----------------------------- LICENSE - GNU GPL v3 -----------------------------------------------

* This license must be included in any redistribution.

* Copyright (c) 2021 Liviu Istrate, www.programming-electronics-diy.xyz (istrateliviu24@yahoo.com)

* Project URL: https://www.programming-electronics-diy.xyz/2021/02/playing-music-and-tones-using-piezo.html
* Inspired from: https://dragaosemchama.com/en/2019/02/songs-for-arduino

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program. If not, see <https://www.gnu.org/licenses/>.

--------------------------------- END OF LICENSE --------------------------------------------------*/

#ifndef CHIP_TUNES_H
#define CHIP_TUNES_H


/*************************************************************
	SYSTEM SETTINGS
**************************************************************/
#define true	1
#define false	0

#define CHIP_TUNES_TIMER_PRESCALER			8

#define CHIP_TUNES_TIMER1					1
#define CHIP_TUNES_TIMER3					3
#define CHIP_TUNES_TIMER4					4

#define CHANNEL_A							0
#define CHANNEL_B							1

#define __CHIP_TUNES_START_MARKER__			5000
#define __CHIP_TUNES_END_MARKER__			5001
#define __CHIP_TUNES_GOTO_MARKER__			5002

/*************************************************************
	INCLUDES
**************************************************************/
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include "pitches.h"
#include "tunes.h"


/*************************************************************
	USER SETUP SECTION
**************************************************************/
#define CHIP_TUNES_TIMER 		CHIP_TUNES_TIMER1 // Which timer to use [CHIP_TUNES_TIMER0 or CHIP_TUNES_TIMER1]

#define CHIP_TUNES_PIN1_DDR 	DDRD
#define CHIP_TUNES_PIN1_PORT 	PORTD
#define CHIP_TUNES_PIN1_PIN  	PD4

// Comment this out if only one pin is used
//#define CHIP_TUNES_PIN2_DDR 	DDRB
//#define CHIP_TUNES_PIN2_PORT 	PORTB
//#define CHIP_TUNES_PIN2_PIN  	PB2

#define CHIP_TUNES_CHANNEL		CHANNEL_B	// CHANNEL_A or CHANNEL_B (OCnA or OCnB)		

#define CHIP_TUNES_BLINK_LED	false
#define CHIP_TUNES_LED_DDR		DDRC
#define CHIP_TUNES_LED_PORT		PORTC
#define CHIP_TUNES_LED_PIN		5


/*************************************************************
	SYSTEM SETTINGS
**************************************************************/
#if CHIP_TUNES_TIMER == CHIP_TUNES_TIMER1

	// Timer1
	#define CHIP_TUNES_REG_OCR_A				OCR1A
	#define CHIP_TUNES_REG_OCR_B				OCR1B
	#define CHIP_TUNES_REG_TOP_HIGH				ICR1H
	#define CHIP_TUNES_REG_TOP_LOW				ICR1L
	#define CHIP_TUNES_REG_TCCRB				TCCR1B
	#define CHIP_TUNES_REG_TCNT					TCNT1
	#define CHIP_TUNES_REG_TCCRA				TCCR1A
	#define CHIP_TUNES_REG_TCCRB				TCCR1B
	#define CHIP_TUNES_BIT_COMA1				COM1A1
	#define CHIP_TUNES_BIT_COMA0				COM1A0
	#define CHIP_TUNES_BIT_COMB1				COM1B1
	#define CHIP_TUNES_BIT_COMB0				COM1B0
	#define CHIP_TUNES_BIT_WGM3					WGM13
	
	#if CHIP_TUNES_TIMER_PRESCALER == 8
		#define CHIP_TUNES_TIMER_CLOCKSEL		(_BV(CS11))
	#endif

#elif CHIP_TUNES_TIMER == CHIP_TUNES_TIMER3

	// Timer3
	#define CHIP_TUNES_REG_OCR_A				OCR3A
	#define CHIP_TUNES_REG_OCR_B				OCR3B
	#define CHIP_TUNES_REG_TOP_HIGH				ICR3H
	#define CHIP_TUNES_REG_TOP_LOW				ICR3L
	#define CHIP_TUNES_REG_TCCRB				TCCR3B
	#define CHIP_TUNES_REG_TCNT					TCNT3
	#define CHIP_TUNES_REG_TCCRA				TCCR3A
	#define CHIP_TUNES_REG_TCCRB				TCCR3B
	#define CHIP_TUNES_BIT_COMA1				COM3A1
	#define CHIP_TUNES_BIT_COMA0				COM3A0
	#define CHIP_TUNES_BIT_COMB1				COM3B1
	#define CHIP_TUNES_BIT_COMB0				COM3B0
	#define CHIP_TUNES_BIT_WGM3					WGM33
	
	#if CHIP_TUNES_TIMER_PRESCALER == 8
		#define CHIP_TUNES_TIMER_CLOCKSEL		(_BV(CS31))
	#endif
	
#elif CHIP_TUNES_TIMER == CHIP_TUNES_TIMER4

	// Timer4
	#define CHIP_TUNES_REG_OCR_A				OCR4A
	#define CHIP_TUNES_REG_OCR_B				OCR4B
	#define CHIP_TUNES_REG_TOP_HIGH				ICR4H
	#define CHIP_TUNES_REG_TOP_LOW				ICR4L
	#define CHIP_TUNES_REG_TCCRB				TCCR4B
	#define CHIP_TUNES_REG_TCNT					TCNT4
	#define CHIP_TUNES_REG_TCCRA				TCCR4A
	#define CHIP_TUNES_REG_TCCRB				TCCR4B
	#define CHIP_TUNES_BIT_COMA1				COM4A1
	#define CHIP_TUNES_BIT_COMA0				COM4A0
	#define CHIP_TUNES_BIT_COMB1				COM4B1
	#define CHIP_TUNES_BIT_COMB0				COM4B0
	#define CHIP_TUNES_BIT_WGM3					WGM43
	
	#if CHIP_TUNES_TIMER_PRESCALER == 8
		#define CHIP_TUNES_TIMER_CLOCKSEL		(_BV(CS41))
	#endif

#else
	#error "Bad CHIP_TUNES_TIMER set"
#endif


/*************************************************************
	GLOBAL VARIABLES
**************************************************************/
volatile char CHIP_TUNES_PLAYING = 0;
static char ToneOn = 0;
static volatile char Volume = 50;
int *CurrentSong;
int *StartMarker;
int *GoBackMarker;
static unsigned int Wholenote;
uint16_t delay_ms = 0;


const int *chipTunes_Playlist[] = {
	//star_wars_theme,
	//tetris_theme,
	//fur_elise,
	//cannon_in_d_pachelbel,
	//greensleeves,
	//happy_birthday,
	//ode_to_joy
};

const uint8_t chipTunes_Tempo[] = {
	//108,	// star_wars_theme
	//144, 	// tetris_theme
	//80, 	// fur_elise
	//100,	// cannon_in_d_pachelbel
	//70,	// greensleeves
	//140,	// happy_birthday
	//114	// ode_to_joy
};



/*************************************************************
	FUNCTION PROTOTYPES
**************************************************************/
void chipTunes_Init(void);
void chipTunes_Tone(uint16_t tone, uint16_t duration_ms);
void chipTunes_Play(const int *pMusicNotes, uint8_t tempo);
void chipTunes_Stop(void);
void chipTunes_SetVolume(uint8_t volume);
char chipTunes_IsPlaying(void);
void chipTunes_ISR(void);

void chipTunes_alert_alarm(uint8_t vuvuzela);
void _chipTunes_SetTimer(void);


/*************************************************************
	FUNCTIONS
**************************************************************/
void chipTunes_Init(void){
	CHIP_TUNES_PIN1_DDR |= (1 << CHIP_TUNES_PIN1_PIN);
	
	#ifdef CHIP_TUNES_PIN2_DDR
		CHIP_TUNES_PIN2_DDR |= (1 << CHIP_TUNES_PIN2_PIN);
	#endif
	
	#if CHIP_TUNES_BLINK_LED == 1
		CHIP_TUNES_LED_DDR |= (1 << CHIP_TUNES_LED_PIN);
		CHIP_TUNES_LED_PORT &= ~(1 << CHIP_TUNES_LED_PIN);
	#endif
	
	_chipTunes_SetTimer();
	
	CHIP_TUNES_REG_TCCRB |= (1 << CHIP_TUNES_BIT_WGM3);
}


void _chipTunes_SetTimer(void){
	// Timer settings for 2 pins, e.g. on OC1A and OC1B
	#ifdef CHIP_TUNES_PIN2_DDR
		// Enable timer 1 in mode 8, PWM, Phase and Frequency Correct (TOP value is ICR1)
		// OC1B - Same as OC1A but inverted
		CHIP_TUNES_REG_TCCRA |= (1 << CHIP_TUNES_BIT_COMA1) | (1 << CHIP_TUNES_BIT_COMB0) | (1 << CHIP_TUNES_BIT_COMB1);

	// Timer settings for 1 pin
	#else
		#if CHIP_TUNES_CHANNEL == CHANNEL_A
			// Enable timer 4 in mode 8, PWM, Phase and Frequency Correct (TOP value is ICR1)
			CHIP_TUNES_REG_TCCRA |= (1 << CHIP_TUNES_BIT_COMA1) | (1 << CHIP_TUNES_BIT_COMA0);
		#elif CHIP_TUNES_CHANNEL == CHANNEL_B
			// Enable timer 4 in mode 8, PWM, Phase and Frequency Correct (TOP value is ICR1)
			CHIP_TUNES_REG_TCCRA |= (1 << CHIP_TUNES_BIT_COMB1) | (1 << CHIP_TUNES_BIT_COMB0);
		#endif
	#endif
}


void chipTunes_Tone(uint16_t tone, uint16_t duration_ms){
	CHIP_TUNES_PLAYING = 1;
	
	// Calculate PWM top value to set the frequency
	uint16_t pwm_top_val = F_CPU / (2 * CHIP_TUNES_TIMER_PRESCALER * (uint32_t)tone); // Mode 8, PWM, Phase and Frequency Correct
	
	// Set duty cycle at 50%
	#ifdef CHIP_TUNES_PIN2_DDR
		CHIP_TUNES_REG_OCR_A = CHIP_TUNES_REG_OCR_B = (pwm_top_val * Volume) / 100;
	#else
		#if CHIP_TUNES_CHANNEL == CHANNEL_A
			CHIP_TUNES_REG_OCR_A = (pwm_top_val * Volume) / 100;
		#elif CHIP_TUNES_CHANNEL == CHANNEL_B
			CHIP_TUNES_REG_OCR_B = (pwm_top_val * Volume) / 100;
		#endif
	#endif
	
	// Set frequency
	CHIP_TUNES_REG_TOP_HIGH = (pwm_top_val >> 8);
	CHIP_TUNES_REG_TOP_LOW = pwm_top_val;
	
	// Start timer
	_chipTunes_SetTimer();
	CHIP_TUNES_REG_TCCRB |= CHIP_TUNES_TIMER_CLOCKSEL;
	
	if(duration_ms){
		delay_ms = duration_ms;
		ToneOn = 1;
	}
}


void chipTunes_Play(const int *pMusicNotes, uint8_t tempo){
	if(CHIP_TUNES_PLAYING == 0){
		CHIP_TUNES_PLAYING = 1;
		
		// Save current song and tempo
		CurrentSong = (int *)pMusicNotes;
		
		// Calculate the duration of a whole note in ms (60s/tempo)*4 beats
		Wholenote = ((uint32_t)60000 * 4) / tempo;
		
		// Start timer
		_chipTunes_SetTimer();
		CHIP_TUNES_REG_TCCRB |= CHIP_TUNES_TIMER_CLOCKSEL;
	}
}


void chipTunes_Stop(void){
	// Stop timer
	CHIP_TUNES_REG_TCCRB &= ~CHIP_TUNES_TIMER_CLOCKSEL;
	
	#if CHIP_TUNES_CHANNEL == CHANNEL_A
		// Enable timer 4 in mode 8, PWM, Phase and Frequency Correct (TOP value is ICR1)
		CHIP_TUNES_REG_TCCRA &= ~((1 << CHIP_TUNES_BIT_COMA1) | (1 << CHIP_TUNES_BIT_COMA0));
	#elif CHIP_TUNES_CHANNEL == CHANNEL_B
		// Enable timer 4 in mode 8, PWM, Phase and Frequency Correct (TOP value is ICR1)
		CHIP_TUNES_REG_TCCRA &= ~((1 << CHIP_TUNES_BIT_COMB1) | (1 << CHIP_TUNES_BIT_COMB0));
	#endif
	
	#ifdef CHIP_TUNES_PIN2_DDR
		CHIP_TUNES_REG_OCR_A = 0;
		CHIP_TUNES_REG_OCR_B = 0xFFFF;
	#else
		#if CHIP_TUNES_CHANNEL == CHANNEL_A
			CHIP_TUNES_REG_OCR_A = 0;
		#elif CHIP_TUNES_CHANNEL == CHANNEL_B
			CHIP_TUNES_REG_OCR_B = 0;
		#endif
	#endif

	CHIP_TUNES_REG_TCNT = 0;
	CHIP_TUNES_PIN1_PORT &= ~(1 << CHIP_TUNES_PIN1_PIN);
	
	CHIP_TUNES_PLAYING = 0;
	
	#if CHIP_TUNES_BLINK_LED == 1
		CHIP_TUNES_LED_PORT &= ~(1 << CHIP_TUNES_LED_PIN);	
	#endif
}


void chipTunes_SetVolume(uint8_t volume){
	if(volume > 50) volume = 50;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		Volume = volume;
	}
}


char chipTunes_IsPlaying(void){
	return CHIP_TUNES_PLAYING;
}



void chipTunes_alert_alarm(uint8_t vuvuzela){
	uint16_t f;
	
	for(f = 100; f < 1000; f++){
		_delay_ms(1);
		
		// This prevents stuttering
		if(vuvuzela == 0) while(CHIP_TUNES_REG_TCNT > 0);
		
		chipTunes_Tone(f, 0);
		
		// Reset timer
		if(vuvuzela) CHIP_TUNES_REG_TCNT = 0;
	}
	
	chipTunes_Stop();
	_delay_ms(200);
}



void chipTunes_ISR(void){
	
	static char repeat_fragment = 0;
	static int16_t duration = 0;
	uint16_t note = 0;
	uint16_t pwm_top_val = 0;
	
	#if CHIP_TUNES_BLINK_LED == 1
		static uint16_t led_delay_ms = 0;
	#endif
	
	if(delay_ms == 0){
		// 150us @ 8MHz with led
		// 118us @ 8MHz no led
		
		if(ToneOn){
			ToneOn = 0;
			chipTunes_Stop();
			return;
		}
		
		// Check if the array pointer is not at the end
		if(pgm_read_word(CurrentSong)){
			note = pgm_read_word(CurrentSong);
			
			// Save the start address where the fragment starts
			if(note == __CHIP_TUNES_START_MARKER__){
				
				// This is not a note - move pointer to a note
				CurrentSong++;
				note = pgm_read_word(CurrentSong);
				StartMarker = CurrentSong;
				
			// Save the end address where the fragment ends	
			}else if(note == __CHIP_TUNES_END_MARKER__){
				
				if(repeat_fragment == 0){
					// This is not a note - move pointer to a note
					CurrentSong++;
					note = pgm_read_word(CurrentSong);
				}else{
					repeat_fragment = 0;
					
					// Go back to where it was before repeating the fragment
					if(pgm_read_word(GoBackMarker)){
						CurrentSong = GoBackMarker;
						note = pgm_read_word(CurrentSong);
					}else{
						chipTunes_Stop();
						return;
					}
				}
			}
			
			// Repeat the fragment
			if(note == __CHIP_TUNES_GOTO_MARKER__){
				// Save the current position
				CurrentSong++;
				GoBackMarker = CurrentSong;
				repeat_fragment = 1;
				
				// Move pointer to the start of the fragment
				CurrentSong = StartMarker;
				note = pgm_read_word(CurrentSong);
			}
			
			
			// Move to the next array element to find duration
			CurrentSong++;
			
			// Get note duration
			// If is greater than 1 (not pause) and less than 33 (not a note) then is duration
			if((int16_t)pgm_read_word(CurrentSong) < 33 && (int16_t)pgm_read_word(CurrentSong) != REST && (int16_t)pgm_read_word(CurrentSong) != 0){
				duration = (int16_t)pgm_read_word(CurrentSong);
				
				if(duration > 0){
					duration = Wholenote / duration;
				}else{
					// Dotted notes are represented with negative durations
					duration = Wholenote / (0 - duration); // 0 - duration: convert duration from negative to positive
					
					// Increases the duration with one half for dotted notes
					duration = duration + (duration / 2);
				}
				
				// Go to next note for next time
				CurrentSong++;
			}
			
			
			if(note == REST){
				// Pause, do not generate any sound
				#ifdef CHIP_TUNES_PIN2_DDR
					CHIP_TUNES_REG_OCR_A = 0;
					CHIP_TUNES_REG_OCR_B = 0xFFFF;
				#else
					#if CHIP_TUNES_CHANNEL == CHANNEL_A
						CHIP_TUNES_REG_OCR_A = 0;
					#elif CHIP_TUNES_CHANNEL == CHANNEL_B
						CHIP_TUNES_REG_OCR_B = 0;
					#endif
				#endif
				
			}else{
				
				// Calculate PWM top value to set the frequency
				pwm_top_val = F_CPU / (2 * CHIP_TUNES_TIMER_PRESCALER * (uint32_t)note); // Mode 8, PWM, Phase and Frequency Correct
				
				// Set duty cycle (volume) 50% duty cycle
				#ifdef CHIP_TUNES_PIN2_DDR
					CHIP_TUNES_REG_OCR_A = CHIP_TUNES_REG_OCR_B = (pwm_top_val * Volume) / 100;
				#else
					#if CHIP_TUNES_CHANNEL == CHANNEL_A
						CHIP_TUNES_REG_OCR_A = (pwm_top_val * Volume) / 100;
					#elif CHIP_TUNES_CHANNEL == CHANNEL_B
						CHIP_TUNES_REG_OCR_B = (pwm_top_val * Volume) / 100;
					#endif
				#endif
				
				// Set frequency
				CHIP_TUNES_REG_TOP_HIGH = (pwm_top_val >> 8);
				CHIP_TUNES_REG_TOP_LOW = pwm_top_val;
				
				// Reset timer
				CHIP_TUNES_REG_TCNT = 0;
			}
			
			
			// Set note or rest delay
			delay_ms = duration;
			
			#if CHIP_TUNES_BLINK_LED == 1
				if(note != REST){
					CHIP_TUNES_LED_PORT |= (1 << CHIP_TUNES_LED_PIN);
					led_delay_ms = ((uint32_t)duration * 90) / 100;
				}
			#endif
			
		}else{
		
			chipTunes_Stop();
		}
		
	}else{
	
		delay_ms--;
		
		#if CHIP_TUNES_BLINK_LED == 1
			if(led_delay_ms > 0){
				led_delay_ms--;
			}else{
				CHIP_TUNES_LED_PORT &= ~(1 << CHIP_TUNES_LED_PIN);
			}
		
		#endif
	}
	
}

#endif