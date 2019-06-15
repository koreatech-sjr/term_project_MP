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

static int customClockSettingIndex = 0;
static int customClock[6] = {0, };
// SET기능 선택시 초기 화면 세팅용 플래그 0이면 초기 진입
static int customClockSetInitFlag = 0; 
// clock용 변수
static int second = 0;
static int minute = 0;
static int hour = 0;


void setClock(int interruptSecond, int segmentStopwatchInitFlag, int* alarmStore) {
	second += interruptSecond;
	minute += second/60; 
	hour += minute/60;
	
	// 24, 60 안넘기기 위해 값 변경
	second = second%60;
	minute = minute%60;
	hour = hour%24;
	
	if(alarmStore[0] == hour && alarmStore[1] == minute && alarmStore[2] == second ){
		//TODO: dotmatrix 추가
		LcdMove(1,0);
		LcdPuts("Alarm Alert");
	}
	// STOPWATCH에서 LCD 사용중인 경우 미노출
	if(segmentStopwatchInitFlag == 0) {
		LcdMove(0,0);
		LcdPutchar((hour/10%10) + '0');
		LcdPutchar(hour%10 + '0');
		LcdPutchar('h');
		
		LcdMove(0,4);
		LcdPutchar((minute/10%10) + '0');
		LcdPutchar(minute%10 + '0');
		LcdPutchar('m');
		
		LcdMove(0,8);
		LcdPutchar((second/10%10) + '0');
		LcdPutchar(second%10 + '0');
		LcdPutchar('s');
	}
}

// 사용자에게 입력 받아 시계 세팅
int setCustomClock(int* ClockCounter_SECOND) {
	// -1 : 숫자가 눌린 경우(전) , 0: 선택이 눌린 경우, 1: 숫자가 눌린 경우(후), 위로 눌린 경우
	int returnStatus = -1;
	// 키를 입력받는 변수
	unsigned char keyInput = NO_KEY;	
	keyInput = KeyInput();
	
	LcdMove(1,customClockSettingIndex);
	while(1) {
		if(customClockSetInitFlag == 0){
			LcdMove(1,0);
			LcdPuts("______");
			LcdMove(1,0);
			customClockSetInitFlag = 1;
		}
		
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
				customClock[customClockSettingIndex] = 1;
				break;
				
			case SW1:				//위와 같음
				LcdPutchar('2');
				customClock[customClockSettingIndex] = 2;
				break;
			
			case SW2:
				LcdPutchar('3');
				customClock[customClockSettingIndex] = 3;
				break;
			
			case SW4:
				LcdPutchar('4');
				customClock[customClockSettingIndex] = 4;
				break;
			
			case SW5:
				LcdPutchar('5');
				customClock[customClockSettingIndex] = 5;
				break;
			
			case SW6:
				LcdPutchar('6');
				customClock[customClockSettingIndex] = 6;
				break;
			
			case SW8:
				LcdPutchar('7');
				customClock[customClockSettingIndex] = 7;
				break;
			
			case SW9:
				LcdPutchar('8');
				customClock[customClockSettingIndex] = 8;
				break;
			
			case SW10:
				LcdPutchar('9');
				customClock[customClockSettingIndex] = 9;
				break;
			
			case SW13:
				LcdPutchar('0');
				customClock[customClockSettingIndex] = 0;
				break;
			
			case SW11:				
				LcdMove(1,0);
				LcdPuts("1. SET          ");				
				returnStatus = 0;
				break;
				
			// 위로 키 입력되는 케이스
			case SW3:
				LcdMove(1, --customClockSettingIndex);
				LcdPutchar('_');
				returnStatus = 1;
				break;
				
			default:
				returnStatus = 1;
				break;
		}
		// index 처리 
		if(returnStatus == -1) {
			customClockSettingIndex++;
			returnStatus = 1;
		}
		// 6자리 입력 후 확인이 눌린 경우
		else if(returnStatus == 0) {
			customClockSettingIndex = 0;
			//클럭 카운터로 증가한 초 초기화
			*ClockCounter_SECOND = 0;
			// 입력받은 시간 초로 변경 후 현재 시계로 초기화 
			hour = customClock[0] * 10 + customClock[1];
			minute = customClock[2] * 10 + customClock[3];
			second = customClock[4] * 10 + customClock[5];
			customClockSetInitFlag = 0;
		}
		return returnStatus;
		
	}
}
