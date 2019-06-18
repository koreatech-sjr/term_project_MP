/*
 * alarm.c
 *
 * Created: 2019-06-14 오전 12:41:40
 *  Author: kadam
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../lib/lcd.h"
#include "../lib/keypad.h"
#include "../lib/iseg7.h"
#include "./alarm.h"

static int customAlarmSettingIndex = 0;
static int customAlarm[12] = {0,};
// timer메뉴 선택시 초기 화면 세팅용 플래그 ; 0이면 초기 진입
static int customAlarmSetInitFlag = 0;
static int customAlarmSetDoneFlag = 0;

static int alarmHour = 0;
static int alarmMinute = 0;
static int alarmSecond = 0;
static int* alarmTimeArray[3] = {0,};
	
int setAlarm() {
	int returnStatus = -1;
	
	// 처음 진입시에만 실행
	if(customAlarmSetInitFlag == 0){
		// 초기화면 세팅
		LcdMove(0,0);
		LcdPuts("00h 00m 00s        ");
		LcdCommand(HOME);
		LcdCommand(CURSOR_ON);
		customAlarmSetInitFlag = 1;
	}
	
	unsigned char keyInput = NO_KEY;
	keyInput = KeyInput();
	
	while(1) {
		switch(keyInput) {
			case SW0:
				LcdPutchar('1');
				customAlarmSettingIndex++;
				customAlarm[customAlarmSettingIndex] = 1;
				break;
			
			case SW1:
				LcdPutchar('2');
				customAlarmSettingIndex++;
				customAlarm[customAlarmSettingIndex] = 2;
				break;
			
			case SW2:
				LcdPutchar('3');
				customAlarmSettingIndex++;
				customAlarm[customAlarmSettingIndex] = 3;
				break;
			
			case SW4:
				LcdPutchar('4');
				customAlarmSettingIndex++;
				customAlarm[customAlarmSettingIndex] = 4;
				break;
			
			case SW5:
				LcdPutchar('5');
				customAlarmSettingIndex++;
				customAlarm[customAlarmSettingIndex] = 5;
				break;
			
			case SW6:
				LcdPutchar('6');
				customAlarmSettingIndex++;
				customAlarm[customAlarmSettingIndex] = 6;
				break;
			
			case SW8:
				LcdPutchar('7');
				customAlarmSettingIndex++;
				customAlarm[customAlarmSettingIndex] = 7;
				break;
			
			case SW9:
				LcdPutchar('8');
				customAlarmSettingIndex++;
				customAlarm[customAlarmSettingIndex] = 8;
				break;
			
			case SW10:
				LcdPutchar('9');
				customAlarmSettingIndex++;
				customAlarm[customAlarmSettingIndex] = 9;
				break;
			
			case SW13:
				LcdPutchar('0');
				customAlarmSettingIndex++;
				customAlarm[customAlarmSettingIndex] = 0;
				break;
			
			// 확인 눌리는 경우
			case SW11:
				returnStatus = 0;
				break;
			
			// 위로 키 입력되는 케이스
			case SW3:
				LcdCommand(CURSOR_LSHIFT);
				customAlarmSettingIndex -= 1;
				customAlarm[customAlarmSettingIndex] = 0;
				returnStatus = 4;
				break;
			
			// 아래 키 입력되는 케이스
			case SW7:
				customAlarmSettingIndex += 1;
				LcdCommand(CURSOR_RSHIFT);
				customAlarm[customAlarmSettingIndex] = 0;
				returnStatus = 4;
				break;
				
			case SW15:
				return -99;
				break;
			default:
				returnStatus = 4;
				break;
		}
		//////////////////////////////////////////////////////////////////////////
		if(returnStatus == -1) {
			returnStatus = 4;
		}
		//확인이 눌린 경우
		else if(returnStatus == 0) {
			customAlarmSetInitFlag = 0;
			LcdMove(0,0);
			LcdPuts("                      ");
			LcdCommand(CURSOR_OFF);
			
			// 시간 계산
			alarmHour = customAlarm[1]*10 + customAlarm[2];
			alarmMinute = customAlarm[5]*10 + customAlarm[6];
			alarmSecond = customAlarm[9]*10 + customAlarm[10];
			
			alarmTimeArray[0] = alarmHour;
			alarmTimeArray[1] = alarmMinute;
			alarmTimeArray[2] = alarmSecond;
			customAlarmSetDoneFlag = 1;
			customAlarmSettingIndex = 0;
			returnStatus = 0;
			break;
		}
		return returnStatus;
	}
}

int* getAlarmTime() {
	if(customAlarmSetDoneFlag == 1){
		customAlarmSetDoneFlag = 0;
		return alarmTimeArray;
	}
	else {
		int *tmp[3] = {-1,};
		return tmp;
	}
}