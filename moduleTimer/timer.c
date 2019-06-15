/*
 * timer.c
 *
 * Created: 2019-06-14 오전 12:40:58
 *  Author: kadam
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../lib/lcd.h"
#include "../lib/keypad.h"
#include "../lib/iseg7.h"
#include "./timer.h"

static int customTimerSettingIndex = 0;
static int customTimer[8] = {0,0,0,0,0,0,0,0};
// timer메뉴 선택시 초기 화면 세팅용 플래그 ; 0이면 초기 진입
static int customTimerSetInitFlag = 0;

// LCD POS (0,0)
static int minute = 0;
// LCD POS (0,3) LCD POS (0,4)
static int second = 0;
// LCD POS (0,6)
static int m_second = 0;

// 7세그먼트 출력용 숫자
unsigned short segmentDisplayNumber = 0;

int setTimer(int* timerInitFlag) {
	int returnStatus = -1;
	
	// 처음 진입시에만 실행
	if(customTimerSetInitFlag == 0){
		// 초기화면 세팅
		LcdMove(0,0);
		LcdPuts("0m 00.0s     ");
		LcdCommand(HOME);
		LcdCommand(CURSOR_ON);
		customTimerSetInitFlag = 1;
	}
	
	unsigned char keyInput = NO_KEY;	
	keyInput = KeyInput();
	
	while(1) {
		switch(keyInput) {
			case SW0:
				LcdPutchar('1');
				customTimerSettingIndex++;
				customTimer[customTimerSettingIndex] = 1;
				break;
			
			case SW1:				
				LcdPutchar('2');
				customTimerSettingIndex++;
				customTimer[customTimerSettingIndex] = 2;
				break;
			
			case SW2:
				LcdPutchar('3');
				customTimerSettingIndex++;
				customTimer[customTimerSettingIndex] = 3;
				break;
			
			case SW4:
				LcdPutchar('4');
				customTimerSettingIndex++;
				customTimer[customTimerSettingIndex] = 4;
				break;
			
			case SW5:
				LcdPutchar('5');
				customTimerSettingIndex++;
				customTimer[customTimerSettingIndex] = 5;
				break;
			
			case SW6:
				LcdPutchar('6');
				customTimerSettingIndex++;
				customTimer[customTimerSettingIndex] = 6;
				break;
			
			case SW8:
				LcdPutchar('7');
				customTimerSettingIndex++;
				customTimer[customTimerSettingIndex] = 7;
				break;
			
			case SW9:
				LcdPutchar('8');
				customTimerSettingIndex++;
				customTimer[customTimerSettingIndex] = 8;
				break;
			
			case SW10:
				LcdPutchar('9');
				customTimerSettingIndex++;
				customTimer[customTimerSettingIndex] = 9;
				break;
			
			case SW13:
				LcdPutchar('0');
				customTimerSettingIndex++;
				customTimer[customTimerSettingIndex] = 0;
				break;
			
			case SW11:
				returnStatus = 0;
				break;
			
			// 위로 키 입력되는 케이스
			case SW3:
				LcdCommand(CURSOR_LSHIFT);
				customTimerSettingIndex -= 1;
				customTimer[customTimerSettingIndex] = 0;
				returnStatus = 2;
				break;
				
			// 아래 키 입력되는 케이스
			case SW7:
				customTimerSettingIndex += 1;
				LcdCommand(CURSOR_RSHIFT);
				customTimer[customTimerSettingIndex] = 0;
				returnStatus = 2;
				break;
			
			default:
				returnStatus = 2;
				break;
		}
		//////////////////////////////////////////////////////////////////////////
		if(returnStatus == -1) {
			returnStatus = 2;
		}
		//확인이 눌린 경우
		else if(returnStatus == 0) {
			customTimerSetInitFlag = 0;
			LcdMove(0,0);
			LcdPuts("                      ");
			LcdCommand(CURSOR_OFF);
			// 시간 계산
			minute = customTimer[0];
			second = customTimer[3] * 10 + customTimer[4];
			m_second = customTimer[6];
			
			// 7 세그먼트 계산
			ISeg7Init();
			segmentDisplayNumber = customTimer[1]*1000 + customTimer[4]*100 + customTimer[5]*10 + customTimer[7];
			ISeg7DispNum(segmentDisplayNumber , 10);
				
			customTimerSettingIndex = 0;
			returnStatus = 0;
			*timerInitFlag = 1;
			break;
		}
		return returnStatus;
	}
}

int set7SegmentTimer(int counter_m_seoncd) {	
	int nowDisplayNumber = segmentDisplayNumber - counter_m_seoncd;
	//1분 00.0초
	//9분 99.9초
	if(nowDisplayNumber%1000 == 999 ){
		nowDisplayNumber -= 400;
	}
	segmentDisplayNumber = nowDisplayNumber;
	ISeg7DispNum(nowDisplayNumber , 10);
	// 타이머 다됨
	if(nowDisplayNumber == 0) {
		// do something
		
		return 0;	
	}
	return 1;
}