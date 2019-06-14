#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "keypad.h"

#define N_COL	4		//the number of keypad's column
#define KEY_OUT	PORTF	//keyboard output port
#define KEY_IN	PINF	//keyboard input port
#define KEY_DIR	DDRF	//keyboard in, output direction

static unsigned char key_scan(void);
static unsigned char pin = NO_KEY;		//previous key value

void KeyInit()	//Initialize keypad
{
	KEY_OUT = 0xF0;		// Initialize output as 0
	KEY_DIR = 0x0F;		//high-ranking nibble = input, low-ranking nibble = output
}

//==========================================================================
// return keypad's scan code
// value of return :
//                   !=0 => value of scan code
//					  =0 => no input
//==========================================================================
unsigned char KeyInput(void)
{
	unsigned char in, in1;
	
	in = key_scan();	//read keys
	while(1)
	{
		_delay_ms(10); _delay_ms(10); _delay_ms(10);
		in1 = key_scan();	//read keys one more time
		if(in == in1) break;
		in = in1;
	}
	
	if (!(in & 0xF0))	// No key was pushed
	{
		pin = 0;
		return 0;
	}
	if(pin == in)
	return 0;
	
	pin = in;		//store key value
	return in;		//return code
}

static unsigned char key_scan(void)
{
	unsigned char out, i , in;
	
	out = 0x01;		// scan from 3rd column
	for(i=0; i<N_COL; i++)
	{
		KEY_OUT = ~out;	//scanned output
		asm("nop"::);
		
		in = (~KEY_IN) & 0xF0;
		if(in)			//have key input
		{
			in += out;	//make a code
			break;
		}
		out <<= 1;	//change to next scan-code
		
	}
	return in;	//return the value of scan-code
}

unsigned char KeyInput_Press(unsigned char* pressed)
{
	static unsigned char press_cnt = 0;
	unsigned char in, in1;
	
	in = key_scan();	// accept keypad input
	
	while(1)		// stabilize switch input
	{
		_delay_ms(10); _delay_ms(10); _delay_ms(10);	//debouncing
		in1 = key_scan();				//accept keypad input on more time
		if(in == in1) break;			//if in == in1, it is stabilized
		in = in1;						//if in != in1, repeat the loop
	}

	*pressed = 0;
	
	if(in == pin)
	{
		press_cnt++;	//increase of pressed time
		if(press_cnt > 16)
		{
			*pressed = 1;		//still pressed
			_delay_ms(10); _delay_ms(10); _delay_ms(10);
			_delay_ms(10); _delay_ms(10); _delay_ms(10);
			_delay_ms(10);		//70msec delay
		}
		else { // pressed in short time -> no input
			return 0;
		}
	}
	else {	// New button has pressed
		press_cnt = 0;		// Reset pressed time
	}

	pin = in;		//store the previous value
	return in;		//Return the value of switch input
}