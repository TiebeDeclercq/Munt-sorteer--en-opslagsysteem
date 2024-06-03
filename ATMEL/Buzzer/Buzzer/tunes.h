#ifndef TUNES_H
#define TUNES_H

/*************************************************************
	CHIP TUNES
**************************************************************/

// Notes of the melody followed by the duration.
// A 4 means a quarter note, 8 an eighteenth, 16 sixteenth, so on.
// Negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth


// Title: 		tunes.h
// Version:		1.1

/********************* CONTENTS TABLE ************************
	1. MOVIES SECTION

	1.1		Dart Vader theme (Imperial March) - Star wars


	2. GAMES SECTION

	2.1		Tetris theme - (Korobeiniki)
	2.2		Mario Main Theme
	2.3		Mario Underworld Melody


	3. CLASSIC SECTION

	3.1		Fur Elise - Ludwig van Beethovem
	3.2		Cannon in D - Pachelbel
	3.3		Greensleeves
	3.4		Ode to Joy - Beethoven's Symphony No. 9


	4. OTHERS SECTION
	
	4.1		Happy Birthday
	
********************* END OF CONTENTS TABLE ********************/

#define CHIPTUNES_INCLUDE_1_1		1

#define CHIPTUNES_INCLUDE_2_1		1
#define CHIPTUNES_INCLUDE_2_2		1
#define CHIPTUNES_INCLUDE_2_3		1

#define CHIPTUNES_INCLUDE_3_1		1
#define CHIPTUNES_INCLUDE_3_2		1
#define CHIPTUNES_INCLUDE_3_3		1
#define CHIPTUNES_INCLUDE_3_4		1

#define CHIPTUNES_INCLUDE_4_1		1


/***************************************************************************************************************************
	MOVIES
****************************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------
*	Title: 		Dart Vader theme (Imperial March) - Star wars
*
*	Tempo: 		108
* 
*	Source: 	https://github.com/robsoncouto/arduino-songs/blob/master/starwars/starwars.ino
*
*	Score available at https://musescore.com/user/202909/scores/1141521
*	The tenor saxophone part was used
*							
*---------------------------------------------------------------------------------------------------------------*/

#if CHIPTUNES_INCLUDE_1_1 == 1

const int star_wars_theme[] PROGMEM = {
	NOTE_AS4,8,		NOTE_AS4,		NOTE_AS4,															//1
	NOTE_F5,2,		NOTE_C6,
	NOTE_AS5,8,		NOTE_A5,		NOTE_G5,		NOTE_F6,2,		NOTE_C6,4,  
	NOTE_AS5,8,		NOTE_A5,		NOTE_G5,		NOTE_F6,2,		NOTE_C6,4,  
	NOTE_AS5,8,		NOTE_A5,		NOTE_AS5,		NOTE_G5,2,		NOTE_C5,8,		NOTE_C5,	NOTE_C5,
	NOTE_F5,2,		NOTE_C6,
	NOTE_AS5,8,		NOTE_A5,		NOTE_G5,		NOTE_F6,2,		NOTE_C6,4,  

	NOTE_AS5,8,		NOTE_A5,		NOTE_G5,		NOTE_F6,2,		NOTE_C6,4,							//8  
	NOTE_AS5,8,		NOTE_A5,		NOTE_AS5,		NOTE_G5,2,		NOTE_C5,-8,		NOTE_C5,16, 
	NOTE_D5,-4,		NOTE_D5,8,		NOTE_AS5,		NOTE_A5,		NOTE_G5,		NOTE_F5,
	NOTE_F5,		NOTE_G5,		NOTE_A5,		NOTE_G5,4,		NOTE_D5,8,		NOTE_E5,4,	NOTE_C5,-8,		NOTE_C5,16,
	NOTE_D5,-4,		NOTE_D5,8,		NOTE_AS5,		NOTE_A5,		NOTE_G5,		NOTE_F5,

	NOTE_C6,-8,		NOTE_G5,16,		NOTE_G5,2,		REST,8,		NOTE_C5,								//13
	NOTE_D5,-4,		NOTE_D5,8,		NOTE_AS5,		NOTE_A5,	NOTE_G5,			NOTE_F5,
	NOTE_F5,		NOTE_G5,		NOTE_A5,		NOTE_G5,4,	NOTE_D5,8,			NOTE_E5,4,	NOTE_C6,-8,		NOTE_C6,16,
	NOTE_F6,4,		NOTE_DS6,8,		NOTE_CS6,4,		NOTE_C6,8,	NOTE_AS5,4,			NOTE_GS5,8,	NOTE_G5,4,		NOTE_F5,8,
	NOTE_C6,1,
	MUSIC_END
};

#endif


/***************************************************************************************************************************
	GAMES
****************************************************************************************************************************/


/*--------------------------------------------------------------------------------------------------------------
*	Title: 		Tetris theme - (Korobeiniki)
*
*	Tempo: 		144
* 
*	Source: 	https://dragaosemchama.com/en/2019/02/songs-for-arduino/
*
*	Based on the arrangement at https://www.flutetunes.com/tunes.php?id=192
*							
*---------------------------------------------------------------------------------------------------------------*/

#if CHIPTUNES_INCLUDE_2_1 == 1

const int tetris_theme[] PROGMEM = {
	NOTE_E5, 4,  	NOTE_B4, 8,  	NOTE_C5,  		NOTE_D5, 4,  	NOTE_C5, 8,		NOTE_B4,
	NOTE_A4, 4,  	NOTE_A4, 8,  	NOTE_C5,  		NOTE_E5, 4,  	NOTE_D5, 8,  	NOTE_C5,
	NOTE_B4, -4,  	NOTE_C5, 8,  	NOTE_D5, 4,		NOTE_E5,
	NOTE_C5,  		NOTE_A4,		NOTE_A4, 8,		NOTE_A4, 4,		NOTE_B4, 8,		NOTE_C5,

	NOTE_D5, -4,	NOTE_F5, 8,		NOTE_A5, 4,		NOTE_G5, 8,		NOTE_F5,
	NOTE_E5, -4,	NOTE_C5, 8,		NOTE_E5, 4,		NOTE_D5, 8,		NOTE_C5,
	NOTE_B4, 4,		NOTE_B4, 8,		NOTE_C5,		NOTE_D5, 4,		NOTE_E5,
	NOTE_C5,		NOTE_A4,		NOTE_A4,		REST, 

	NOTE_E5,		NOTE_B4, 8,		NOTE_C5,		NOTE_D5, 4,		NOTE_C5, 8,		NOTE_B4,
	NOTE_A4, 4,		NOTE_A4, 8,		NOTE_C5,		NOTE_E5, 4,		NOTE_D5, 8,		NOTE_C5,
	NOTE_B4, -4,	NOTE_C5, 8,		NOTE_D5, 4,		NOTE_E5,
	NOTE_C5,		NOTE_A4,		NOTE_A4, 8,		NOTE_A4, 4,		NOTE_B4, 8,		NOTE_C5,

	NOTE_D5, -4,	NOTE_F5, 8,		NOTE_A5, 4,		NOTE_G5, 8,		NOTE_F5,
	NOTE_E5, -4,	NOTE_C5, 8,		NOTE_E5, 4,		NOTE_D5, 8,		NOTE_C5,
	NOTE_B4, 4,		NOTE_B4, 8,		NOTE_C5,		NOTE_D5, 4,		NOTE_E5,
	NOTE_C5,		NOTE_A4,		NOTE_A4,		REST,

	NOTE_E5, 2,		NOTE_C5,
	NOTE_D5,		NOTE_B4,
	NOTE_C5,		NOTE_A4,
	NOTE_GS4,		NOTE_B4, 4,		REST, 8, 
	NOTE_E5, 2,		NOTE_C5,
	NOTE_D5,		NOTE_B4,
	NOTE_C5, 4,		NOTE_E5,		NOTE_A5, 2,
	NOTE_GS5,
	MUSIC_END
};

#endif


/*--------------------------------------------------------------------------------------------------------------
*	Title: 		Mario Main Theme
*
*	Tempo: 		120
* 
*	Source: 	http://aquaticus.info/pwm-music
*							
*---------------------------------------------------------------------------------------------------------------*/

#if CHIPTUNES_INCLUDE_2_2 == 1

const int mario_main_theme[] PROGMEM = {
	NOTE_E7,12,		NOTE_E7,	REST,		NOTE_E7,	REST,		NOTE_C7,	NOTE_E7,	REST,
	NOTE_G7,		REST,		REST,		REST,		NOTE_G6,	REST,		REST,		REST,
	NOTE_C7,		REST,		REST,		NOTE_G6,	REST,		REST,		NOTE_E6,	REST,
	REST,			NOTE_A6,	REST,		NOTE_B6,	REST,		NOTE_AS6,	NOTE_A6,	REST,
	NOTE_G6,9,		NOTE_E7,	NOTE_G7,	NOTE_A7,12,	REST,		NOTE_F7,	NOTE_G7,	REST,
	NOTE_E7,		REST,		NOTE_C7,	NOTE_D7,	NOTE_B6,	REST,		REST,		NOTE_C7,
	REST,			REST,		NOTE_G6,	REST,		REST,		NOTE_E6,	REST,		REST,
	NOTE_A6,		REST,		NOTE_B6,	REST,		NOTE_AS6,	NOTE_A6,	REST,		NOTE_G6,9,
	NOTE_E7,		NOTE_G7,	NOTE_A7,12,	REST,		NOTE_F7,	NOTE_G7,	REST,		NOTE_E7,
	REST,			NOTE_C7,	NOTE_D7,	NOTE_B6,	REST,		REST,		NOTE_C7,
	MUSIC_END
};

#endif



/*--------------------------------------------------------------------------------------------------------------
*	Title: 		Mario Underworld Melody
*
*	Tempo: 		120
* 
*	Source: 	http://aquaticus.info/pwm-music
*							
*---------------------------------------------------------------------------------------------------------------*/

#if CHIPTUNES_INCLUDE_2_3 == 1

const int mario_underworld_melody[] PROGMEM = {
	NOTE_C4,12,		NOTE_C5,		NOTE_A3,		NOTE_A4,		NOTE_AS3,		NOTE_AS4,		REST,6,		REST,3,
	NOTE_C4,12,		NOTE_C5,		NOTE_A3,		NOTE_A4,		NOTE_AS3,		NOTE_AS4,		REST,6,		REST,3,
	NOTE_F3,12,		NOTE_F4,		NOTE_D3,		NOTE_D4,		NOTE_D3,		NOTE_DS4,		REST,6,		REST,3,
	NOTE_F3,12,		NOTE_F4,		NOTE_D3,		NOTE_D4,		NOTE_DS3,		NOTE_DS4,		REST,6,		REST,
	NOTE_DS4,18,	NOTE_CS4,		NOTE_D4,		NOTE_CS4,6,		NOTE_DS4,		NOTE_DS4,		NOTE_GS3,	NOTE_G3,
	NOTE_CS4,		NOTE_C4,18,		NOTE_FS4,		NOTE_F4,		NOTE_E3,		NOTE_AS4,		NOTE_A4,	NOTE_GS4,10,
	NOTE_DS4,		NOTE_B3,		NOTE_AS3,		NOTE_A3,		NOTE_GS3,		REST,3,			REST,		REST,
	MUSIC_END
};

#endif



/***************************************************************************************************************************
	CLASSIC
****************************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------
*	Title: 		Fur Elise - Ludwig van Beethovem
*
*	Tempo: 		80
* 
*	Source: 	https://github.com/robsoncouto/arduino-songs/blob/master/furelise/furelise.ino
*
*	Score available at https://musescore.com/user/28149610/scores/5281944
*							
*---------------------------------------------------------------------------------------------------------------*/

#if CHIPTUNES_INCLUDE_3_1 == 1

const int fur_elise[] PROGMEM = {
	// starts from 1 ending on 9
	__CHIP_TUNES_START_MARKER__,
	NOTE_E5, 16,	NOTE_DS5,														//1
	NOTE_E5,		NOTE_DS5,		NOTE_E5,	NOTE_B4,	NOTE_D5,	NOTE_C5,
	NOTE_A4, -8,	NOTE_C4, 16,	NOTE_E4,	NOTE_A4,
	NOTE_B4, -8,	NOTE_E4, 16,	NOTE_GS4,	NOTE_B4,
	NOTE_C5, 8,		REST, 16,		NOTE_E4,	NOTE_E5,	NOTE_DS5,

	NOTE_E5,		NOTE_DS5,		NOTE_E5,	NOTE_B4,	NOTE_D5,	NOTE_C5,	//6
	NOTE_A4, -8,	NOTE_C4, 16,	NOTE_E4,	NOTE_A4,
	NOTE_B4, -8,	NOTE_E4, 16,	NOTE_C5,	NOTE_B4,
	__CHIP_TUNES_END_MARKER__,
	NOTE_A4 , 4,	REST, 8, 														//9 - 1st ending

	// repeats from 1 ending on 10
	__CHIP_TUNES_GOTO_MARKER__,
	NOTE_A4, 8,		REST, 16,		NOTE_B4,	NOTE_C5,	NOTE_D5,				//10 - 2nd ending
	
	// continues from 11
	__CHIP_TUNES_START_MARKER__,
	NOTE_E5, -8,	NOTE_G4, 16,	NOTE_F5,	NOTE_E5,
	NOTE_D5, -8,	NOTE_F4, 16,	NOTE_E5,	NOTE_D5,							//12

	NOTE_C5, -8,	NOTE_E4, 16,	NOTE_D5,	NOTE_C5,							//13
	NOTE_B4, 8,		REST, 16,		NOTE_E4,	NOTE_E5,	REST,
	REST,			NOTE_E5,		NOTE_E6,	REST,		REST,		NOTE_DS5,
	NOTE_E5,		REST,			REST,		NOTE_DS5,	NOTE_E5,	NOTE_DS5,
	NOTE_E5,		NOTE_DS5,		NOTE_E5,	NOTE_B4,	NOTE_D5,	NOTE_C5,
	NOTE_A4, 8,		REST, 16,		NOTE_C4,	NOTE_E4,	NOTE_A4,

	NOTE_B4, 8,		REST, 16,		NOTE_E4,	NOTE_GS4,	NOTE_B4,				//19
	NOTE_C5, 8,		REST, 16,		NOTE_E4,	NOTE_E5,	NOTE_DS5,
	NOTE_E5,		NOTE_DS5,		NOTE_E5,	NOTE_B4,	NOTE_D5,	NOTE_C5,
	NOTE_A4, 8,		REST, 16,		NOTE_C4,	NOTE_E4,	NOTE_A4,
	NOTE_B4, 8,		REST, 16,		NOTE_E4,	NOTE_C5,	NOTE_B4,
	__CHIP_TUNES_END_MARKER__,
	NOTE_A4, 8,		REST, 16,		NOTE_B4,	NOTE_C5,	NOTE_D5,				//24 (1st ending)

	// repeats from 11
	__CHIP_TUNES_GOTO_MARKER__,
	NOTE_A4, 8,		REST, 16,		NOTE_C5,	NOTE_C5,	NOTE_C5,				//25 - 2nd ending

	// continues from 26
	NOTE_C5, 4,		NOTE_F5, -16,	NOTE_E5, 32,									//26
	NOTE_E5, 8,		NOTE_D5,		NOTE_AS5, -16,	NOTE_A5, 32,
	NOTE_A5, 16,	NOTE_G5,		NOTE_F5,		NOTE_E5,		NOTE_D5,		NOTE_C5,
	NOTE_AS4, 8,	NOTE_A4,		NOTE_A4, 32,	NOTE_G4,		NOTE_A4,		NOTE_B4,
	NOTE_C5, 4,		NOTE_D5, 16,	NOTE_DS5,
	NOTE_E5, -8,	NOTE_E5, 16,	NOTE_F5,		NOTE_A4,
	NOTE_C5, 4,		NOTE_D5, -16,	NOTE_B4, 32,


	NOTE_C5,		NOTE_G5,		NOTE_G4,		NOTE_G5,	NOTE_A4,	NOTE_G5,	NOTE_B4,	NOTE_G5,	NOTE_C5,	NOTE_G5,	NOTE_D5,	NOTE_G5,		//33
	NOTE_E5,		NOTE_G5,		NOTE_C6,		NOTE_B5,	NOTE_A5,	NOTE_G5,	NOTE_F5,	NOTE_E5,	NOTE_D5,	NOTE_G5,	NOTE_F5,	NOTE_D5,
	NOTE_C5,		NOTE_G5,		NOTE_G4,		NOTE_G5,	NOTE_A4,	NOTE_G5,	NOTE_B4,	NOTE_G5,	NOTE_C5,	NOTE_G5,	NOTE_D5,	NOTE_G5,

	NOTE_E5,		NOTE_G5,		NOTE_C6,		NOTE_B5,	NOTE_A5,	NOTE_G5,	NOTE_F5,	NOTE_E5,	NOTE_D5,	NOTE_G5,	NOTE_F5,	NOTE_D5,		//36
	NOTE_E5,		NOTE_F5,		NOTE_E5,		NOTE_DS5,	NOTE_E5,	NOTE_B4,	NOTE_E5,	NOTE_DS5,	NOTE_E5,	NOTE_B4,	NOTE_E5,	NOTE_DS5,
	NOTE_E5, -8,	NOTE_B4, 16,	NOTE_E5,		NOTE_DS5,
	NOTE_E5, -8,	NOTE_B4, 16,	NOTE_E5,		REST,

	REST,			NOTE_DS5,		NOTE_E5,		REST,		REST,		NOTE_DS5,		 //40
	NOTE_E5,		NOTE_DS5,		NOTE_E5,		NOTE_B4,	NOTE_D5,	NOTE_C5,
	NOTE_A4, 8,		REST, 16,		NOTE_C4,		NOTE_E4,	NOTE_A4,
	NOTE_B4, 8,		REST, 16,		NOTE_E4,		NOTE_GS4,	NOTE_B4,
	NOTE_C5, 8,		REST, 16,		NOTE_E4,		NOTE_E5,	NOTE_DS5,
	NOTE_E5,		NOTE_DS5,		NOTE_E5,		NOTE_B4,	NOTE_D5,	NOTE_C5,

	NOTE_A4, 8,		REST, 16,		NOTE_C4,		NOTE_E4,	NOTE_A4,					//46
	NOTE_B4, 8,		REST, 16,		NOTE_E4,		NOTE_C5,	NOTE_B4,
	NOTE_A4, 8,		REST, 16,		NOTE_B4,		NOTE_C5,	NOTE_D5,
	NOTE_E5, -8,	NOTE_G4, 16,	NOTE_F5,		NOTE_E5,
	NOTE_D5, -8,	NOTE_F4, 16,	NOTE_E5,		NOTE_D5,
	NOTE_C5, -8,	NOTE_E4, 16,	NOTE_D5,		NOTE_C5,
	NOTE_B4, 8,		REST, 16,		NOTE_E4,		NOTE_E5,	REST,
	REST,			NOTE_E5,		NOTE_E6,		REST,		REST,		NOTE_DS5,

	NOTE_E5,		REST,			REST,			NOTE_DS5,	NOTE_E5,	NOTE_D5,		//54
	NOTE_E5,		NOTE_DS5,		NOTE_E5,		NOTE_B4,	NOTE_D5,	NOTE_C5,
	NOTE_A4, 8,		REST, 16,		NOTE_C4,		NOTE_E4,	NOTE_A4,
	NOTE_B4, 8,		REST, 16,		NOTE_E4,		NOTE_GS4,	NOTE_B4,
	NOTE_C5, 8,		REST, 16,		NOTE_E4,		NOTE_E5,	NOTE_DS5,
	NOTE_E5,		NOTE_DS5,		NOTE_E5,		NOTE_B4,	NOTE_D5,	NOTE_C5,

	NOTE_A4, 8,		REST, 16,		NOTE_C4,		NOTE_E4,	NOTE_A4,					//60
	NOTE_B4, 8,		REST, 16,		NOTE_E4,		NOTE_C5,	NOTE_B4,
	NOTE_A4, 8,		REST, 16,		REST,			REST, 8, 	NOTE_CS5, -4, 
	NOTE_D5, 4,		NOTE_E5, 16,	NOTE_F5,
	NOTE_F5, 4,		NOTE_F5, 8, 	NOTE_E5, -4,
	NOTE_D5, 4,		NOTE_C5, 16,	NOTE_B4,
	NOTE_A4, 4,		NOTE_A4, 8,		NOTE_A4,		NOTE_C5,		NOTE_B4,
	NOTE_A4, -4,	NOTE_CS5, -4,

	NOTE_D5, 4,		NOTE_E5, 16,	NOTE_F5,												//72
	NOTE_F5, 4,		NOTE_F5, 8,		NOTE_F5, -4,
	NOTE_DS5, 4,	NOTE_D5, 16,	NOTE_C5,
	NOTE_AS4, 4,	NOTE_A4, 8,
	NOTE_GS4, 4,	NOTE_G4, 8,		NOTE_A4, -4,
	NOTE_B4, 4,		REST, 8,
	NOTE_A3, -32,	NOTE_C4,		NOTE_E4,		NOTE_A4,	NOTE_C5,	NOTE_E5,	NOTE_D5,	NOTE_C5,	NOTE_B4,

	NOTE_A4,		NOTE_C5,		NOTE_E5,		NOTE_A5,	NOTE_C6,	NOTE_E6,	NOTE_D6, 	NOTE_C6,	NOTE_B5,		//80
	NOTE_A4,		NOTE_C5,		NOTE_E5,		NOTE_A5,	NOTE_C6,	NOTE_E6,	NOTE_D6,	NOTE_C6,	NOTE_B5,
	NOTE_AS5,		NOTE_A5,		NOTE_GS5,		NOTE_G5,	NOTE_FS5,	NOTE_F5,	NOTE_E5,	NOTE_DS5,	NOTE_D5,

	NOTE_CS5,		NOTE_C5,		NOTE_B4,		NOTE_AS4,	NOTE_A4,	NOTE_GS4,	NOTE_G4,	NOTE_FS4,	NOTE_F4,		//84
	NOTE_E4, 16,	NOTE_DS5,		NOTE_E5,		NOTE_B4,	NOTE_D5,	NOTE_C5,
	NOTE_A4, -8,	NOTE_C4, 16,	NOTE_E4,		NOTE_A4,
	NOTE_B4, -8,	NOTE_E4, 16,	NOTE_GS4,		NOTE_B4,

	NOTE_C5, 8,		REST, 16,		NOTE_E4,		NOTE_E5,	NOTE_DS5,														//88
	NOTE_E5,		NOTE_DS5,		NOTE_E5,		NOTE_B4,	NOTE_D5,	NOTE_C5,
	NOTE_A4, -8,	NOTE_C4, 16,	NOTE_E4,		NOTE_A4,
	NOTE_B4, -8,	NOTE_E4, 16,	NOTE_C5,		NOTE_B4,
	NOTE_A4, -8,	REST,			REST,			NOTE_G4, 16,	NOTE_F5,		NOTE_E5,
	NOTE_D5, 4,		REST, 8,		REST, -8,		NOTE_E4, 16,	NOTE_D5,		NOTE_C5,

	NOTE_B4, -8,	NOTE_E4, 16,	NOTE_E5, 8,																					//96
	NOTE_E5,		NOTE_E6, -8,	NOTE_DS5, 16,	NOTE_E5,	REST,		REST,	NOTE_DS5,	NOTE_E5,	NOTE_DS5,
	NOTE_E5,		NOTE_DS5,		NOTE_E5,		NOTE_B4,	NOTE_D5,	NOTE_C5,
	NOTE_A4, -8,	NOTE_C4, 16,	NOTE_E4,		NOTE_A4,
	NOTE_B4, -8,	NOTE_E4, 16,	NOTE_GS4,		NOTE_B4,

	NOTE_C5, 8,		REST, 16,		NOTE_E4,		NOTE_E5,	NOTE_DS5,														//102
	NOTE_E5,		NOTE_DS5,		NOTE_E5,		NOTE_B4,	NOTE_D5,	NOTE_C5,
	NOTE_A4, -8,	NOTE_C4, 16,	NOTE_E4,		NOTE_A4,
	NOTE_B4, -8,	NOTE_E4, 16,	NOTE_C5,		NOTE_B4,	NOTE_A4, -4,
    MUSIC_END
};

#endif


/*--------------------------------------------------------------------------------------------------------------
*	Title: 		Cannon in D - Pachelbel
*
*	Tempo: 		100
* 
*	Source: 	https://github.com/robsoncouto/arduino-songs/blob/master/cannonind/cannonind.ino
*
*	Score available at https://musescore.com/user/4710311/scores/1975521
*							
*---------------------------------------------------------------------------------------------------------------*/

#if CHIPTUNES_INCLUDE_3_2 == 1

const int cannon_in_d_pachelbel[] PROGMEM = {
	// C F
	NOTE_FS4,2,		NOTE_E4,		NOTE_D4,		NOTE_CS4,
	NOTE_B3,		NOTE_A3,		NOTE_B3,		NOTE_CS4,
	NOTE_FS4,		NOTE_E4,		NOTE_D4,		NOTE_CS4,
	NOTE_B3,		NOTE_A3,		NOTE_B3,		NOTE_CS4,
	NOTE_D4,		NOTE_CS4,		NOTE_B3,		NOTE_A3,
	NOTE_G3,		NOTE_FS3,		NOTE_G3,		NOTE_A3,

	NOTE_D4,4,		NOTE_FS4,8,		NOTE_G4,		NOTE_A4,4,		NOTE_FS4,8,		NOTE_G4,
	NOTE_A4,4,		NOTE_B3,8,		NOTE_CS4,		NOTE_D4,		NOTE_E4,		NOTE_FS4,		NOTE_G4,
	NOTE_FS4,4,		NOTE_D4,8,		NOTE_E4,		NOTE_FS4,4,		NOTE_FS3,8,		NOTE_G3,
	NOTE_A3,		NOTE_G3,		NOTE_FS3,		NOTE_G3,		NOTE_A3,2,
	NOTE_G3,4,		NOTE_B3,8,		NOTE_A3,		NOTE_G3,4,		NOTE_FS3,8,		NOTE_E3,
	NOTE_FS3,4,		NOTE_D3,8,		NOTE_E3,		NOTE_FS3,		NOTE_G3,		NOTE_A3,		NOTE_B3,

	NOTE_G3,4,		NOTE_B3,8,		NOTE_A3,		NOTE_B3,4,		NOTE_CS4,8,		NOTE_D4,
	NOTE_A3,		NOTE_B3,		NOTE_CS4,		NOTE_D4,		NOTE_E4,		NOTE_FS4,		NOTE_G4,		NOTE_A4,2,
	NOTE_A4,4,		NOTE_FS4,8,		NOTE_G4,		NOTE_A4,4,
	NOTE_FS4,8,		NOTE_G4,		NOTE_A4,		NOTE_A3,		NOTE_B3,		NOTE_CS4,
	NOTE_D4,		NOTE_E4,		NOTE_FS4,		NOTE_G4,		NOTE_FS4,4,		NOTE_D4,8,		NOTE_E4,
	NOTE_FS4,		NOTE_CS4,		NOTE_A3,		NOTE_A3,

	NOTE_CS4,4,		NOTE_B3,		NOTE_D4,8,		NOTE_CS4,		NOTE_B3,4,
	NOTE_A3,8,		NOTE_G3,		NOTE_A3,4,		NOTE_D3,8,		NOTE_E3,		NOTE_FS3,		NOTE_G3,
	NOTE_A3,		NOTE_B3,4,		NOTE_G3,		NOTE_B3,8,		NOTE_A3,		NOTE_B3,4,
	NOTE_CS4,8,		NOTE_D4,		NOTE_A3,		NOTE_B3,		NOTE_CS4,		NOTE_D4,		NOTE_E4,
	NOTE_FS4,		NOTE_G4,		NOTE_A4,2,
	MUSIC_END
};

#endif


/*--------------------------------------------------------------------------------------------------------------
*	Title: 		Greensleeves
*
*	Tempo: 		70
* 
*	Source: 	https://github.com/robsoncouto/arduino-songs/blob/master/greensleeves/greensleeves.ino
*
*	Score available at https://musescore.com/user/168402/scores/1396946
*							
*---------------------------------------------------------------------------------------------------------------*/

#if CHIPTUNES_INCLUDE_3_3 == 1

const int greensleeves[] PROGMEM = {
	__CHIP_TUNES_START_MARKER__,
	NOTE_G4,8,																							//1
	NOTE_AS4,4,		NOTE_C5,8,		NOTE_D5,-8,		NOTE_DS5,16,		NOTE_D5,8,
	NOTE_C5,4,		NOTE_A4,8,		NOTE_F4,-8,		NOTE_G4,16,			NOTE_A4,8,
	NOTE_AS4,4,		NOTE_G4,8,		NOTE_G4,-8,		NOTE_FS4,16,		NOTE_G4,8,
	NOTE_A4,4,		NOTE_FS4,8,		NOTE_D4,4,		NOTE_G4,8,

	NOTE_AS4,4,		NOTE_C5,8,		NOTE_D5,-8,		NOTE_DS5,16,		NOTE_D5,8,						//6
	NOTE_C5,4,		NOTE_A4,8,		NOTE_F4,-8,		NOTE_G4,16,			NOTE_A4,8,
	NOTE_AS4,-8,	NOTE_A4,16,		NOTE_G4,8,		NOTE_FS4,-8,		NOTE_E4,16,		NOTE_FS4,8, 
	NOTE_G4,-2,		NOTE_F5,2,		NOTE_E5,16,		NOTE_D5,8,

	NOTE_C5,4,		NOTE_A4,8,		NOTE_F4,-8,		NOTE_G4,16,			NOTE_A4,8,						//11
	NOTE_AS4,4,		NOTE_G4,8,		NOTE_G4,-8,		NOTE_FS4,16,		NOTE_G4,8,
	NOTE_A4,4,		NOTE_FS4,8,		NOTE_D4,04,		NOTE_F5,2,			NOTE_E5,16,		NOTE_D5,8,
	NOTE_C5,4,		NOTE_A4,8,		NOTE_F4,-8,		NOTE_G4,16,			NOTE_A4,8,

	NOTE_AS4,-8,	NOTE_A4,16,		NOTE_G4,8,		NOTE_FS4,-8,		NOTE_E4,16,		NOTE_FS4,8,		//16
	NOTE_G4,-2,
	__CHIP_TUNES_END_MARKER__,
	__CHIP_TUNES_GOTO_MARKER__,

	//repeats from the beginning
	MUSIC_END
};

#endif



/*--------------------------------------------------------------------------------------------------------------
*	Title: 		Ode to Joy - Beethoven's Symphony No. 9
*
*	Tempo: 		114
* 
*	Source: 	https://github.com/robsoncouto/arduino-songs/blob/master/odetojoy/odetojoy.ino
*							
*---------------------------------------------------------------------------------------------------------------*/

#if CHIPTUNES_INCLUDE_3_4 == 1

const int ode_to_joy[] PROGMEM = {
	NOTE_E4,4,		NOTE_E4,		NOTE_F4,		NOTE_G4,				//1
	NOTE_G4,		NOTE_F4,		NOTE_E4,		NOTE_D4,
	NOTE_C4,		NOTE_C4,		NOTE_D4,		NOTE_E4,
	NOTE_E4,-4,		NOTE_D4,8,		NOTE_D4,2,

	NOTE_E4,4,		NOTE_E4,		NOTE_F4,		NOTE_G4,				//4
	NOTE_G4,		NOTE_F4,		NOTE_E4,		NOTE_D4,
	NOTE_C4,		NOTE_C4,		NOTE_D4,		NOTE_E4,
	NOTE_D4,-4,		NOTE_C4,8,		NOTE_C4,2,

	NOTE_D4,4,		NOTE_D4,		NOTE_E4,		NOTE_C4,				//8
	NOTE_D4,		NOTE_E4,8,		NOTE_F4,		NOTE_E4,4,		NOTE_C4,
	NOTE_D4,		NOTE_E4,8,		NOTE_F4,		NOTE_E4,4,		NOTE_D4,
	NOTE_C4,		NOTE_D4,		NOTE_G3,2,

	NOTE_E4,4,		NOTE_E4,		NOTE_F4,		NOTE_G4,				//12
	NOTE_G4,		NOTE_F4,		NOTE_E4,		NOTE_D4,
	NOTE_C4,		NOTE_C4,		NOTE_D4,		NOTE_E4,
	NOTE_D4,-4,		NOTE_C4,8,		NOTE_C4,2,
	MUSIC_END
};

#endif



/***************************************************************************************************************************
	OTHERS
****************************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------
*	Title: 		Happy Birthday
*
*	Tempo: 		140
* 
*	Source: 	https://github.com/robsoncouto/arduino-songs/blob/master/happybirthday/happybirthday.ino
*
*	Score available at https://musescore.com/user/8221/scores/26906
*							
*---------------------------------------------------------------------------------------------------------------*/

#if CHIPTUNES_INCLUDE_4_1 == 1

const int happy_birthday[] PROGMEM = {
	NOTE_C4,4,		NOTE_C4,8, 
	NOTE_D4,-4,		NOTE_C4,		NOTE_F4,
	NOTE_E4,-2,		NOTE_C4,4,		NOTE_C4,8, 
	NOTE_D4,-4,		NOTE_C4,		NOTE_G4,
	NOTE_F4,-2,		NOTE_C4,4,		NOTE_C4,8,

	NOTE_C5,-4,		NOTE_A4,		NOTE_F4,
	NOTE_E4,		NOTE_D4,		NOTE_AS4,4,		NOTE_AS4,8,
	NOTE_A4,-4,		NOTE_F4,		NOTE_G4,		NOTE_F4,-2,
	MUSIC_END
};

#endif

#endif