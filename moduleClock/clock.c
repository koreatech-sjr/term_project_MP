/*
 * clock.c
 *
 * Created: 2019-06-14 오전 12:40:06
 *  Author: kadam
 */ 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../lib/lcd.h"
#include "../lib/keypad.h"
#include "./clock.h"

static int timerSettingIndex = 0;
static int customTimer[6] = {0, };

// clock용 변수
static int second = 0;
static int minute = 0;
static int hour = 0;

void setClock(int interruptSecond) {
	second += interruptSecond;
	minute += second/60; 
	hour += minute/60;
	
	// 24, 60 안넘기기 위해 값 변경
	second = second%60;
	minute = minute%60;
	hour = hour%24;
	
	LcdMove(0,0);
	LcdPutchar((hour/10%10) + '0');
	LcdPutchar(hour%10 + '0');
	
	LcdMove(0,4);
	LcdPutchar((minute/10%10) + '0');
	LcdPutchar(minute%10 + '0');
	
	LcdMove(0,8);
	LcdPutchar((second/10%10) + '0');
	LcdPutchar(second%10 + '0');
}

// 사용자에게 입력 받아 시계 세팅
int setCustomClock(int* ClockCounter_SECOND) {
//	LcdPuts("000000");
	int returnStatus = -1;
	unsigned char keyInput = NO_KEY;	//키가 눌리지 않으면
	// 키를 입력받는 변수
	keyInput = KeyInput();
	
	LcdMove(1,timerSettingIndex);
	while(1) {
		if(keyInput == NO_KEY){
			LcdCommand(DISP_OFF);
			msec_delay(500);
			LcdCommand(DISP_ON);
			msec_delay(500);
			return 1;
		}
		
		switch(keyInput) {
			case SW0:
				LcdPutchar('1');
				customTimer[timerSettingIndex] = 1;
				break;
				
			case SW1:				//위와 같음
				LcdPutchar('2');
				customTimer[timerSettingIndex] = 2;
				break;
			
			case SW2:
				LcdPutchar('3');
				customTimer[timerSettingIndex] = 3;
				break;
			
			case SW4:
				LcdPutchar('4');
				customTimer[timerSettingIndex] = 4;
				break;
			
			case SW5:
				LcdPutchar('5');
				customTimer[timerSettingIndex] = 5;
				break;
			
			case SW6:
				LcdPutchar('6');
				customTimer[timerSettingIndex] = 6;
				break;
			
			case SW8:
				LcdPutchar('7');
				customTimer[timerSettingIndex] = 7;
				break;
			
			case SW9:
				LcdPutchar('8');
				customTimer[timerSettingIndex] = 8;
				break;
			
			case SW10:
				LcdPutchar('9');
				customTimer[timerSettingIndex] = 9;
				break;
			
			case SW13:
				LcdPutchar('0');
				customTimer[timerSettingIndex] = 0;
				break;
			
			case SW11:				
				LcdMove(1,0);
				LcdPuts("1. SET      ");				
				returnStatus = 0;
				break;
				
			default:
				returnStatus = 1;
				break;
		}
		// index 처리 
		if(returnStatus == -1) {
			timerSettingIndex++;
			returnStatus = 1;
		}
		// 6자리 입력 후 확인이 눌린 경우
		else if(returnStatus == 0) {
			timerSettingIndex = 0;
			*ClockCounter_SECOND = 0;
			// 입력받은 시간 초로 변경 후 현재 시계로 초기화 
			hour = customTimer[0] * 10 + customTimer[1];
			minute = customTimer[2] * 10 + customTimer[3];
			second = customTimer[4] * 10 + customTimer[5];
			
		}
		return returnStatus;
		
	}
}
