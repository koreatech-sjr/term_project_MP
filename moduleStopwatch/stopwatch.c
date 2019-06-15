/*
 * stopwatch.c
 *
 * Created: 2019-06-14 오전 12:41:10
 *  Author: kadam
 */ 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../lib/lcd.h"
#include "../lib/keypad.h"
#include "../lib/iseg7.h"
#include "./stopwatch.h"

// 7세그먼트 출력용 숫자
unsigned short segmentStopwatchDisplayNumber = 0;
// stopwatch 기능 선택시 초기 화면 세팅용 플래그 ; 0이면 초기 진입
static int customStopwatchSetInitFlag = 0;

static int lcdIndx = 0;

int setStopwatch(int* stopwatchInitFlag) {
	int returnStatus = -1;
	// 처음 진입시에만 실행
	if(customStopwatchSetInitFlag == 0) {
		ISeg7Init();
		ISeg7DispNum(segmentStopwatchDisplayNumber , 10);
		// 초기화면 세팅
		LcdMove(0,0);
		LcdPuts("0m 00.0s     ");
	}
	customStopwatchSetInitFlag = 1;

	unsigned char keyInput = NO_KEY;
	keyInput = KeyInput();
	ISeg7DispNum(segmentStopwatchDisplayNumber, 10);
	while(1) {
		switch(keyInput) {
			// 확인 키 누름
			case SW11:
				*stopwatchInitFlag = 1;
				returnStatus = 0;
				LcdMove(0,0);
				LcdPuts("              ");
				
				LcdMove(1,0);
				LcdPuts("              ");
				
				LcdMove(0,0);
				break;
			
			default:
				returnStatus = 3;
				break;
		}
		//확인이 눌린 경우
		if(returnStatus == 0) {
			
		}
		return returnStatus;
	}
}

int set7SegmentStopwatch(int clock_counter, unsigned char stopwatchKeyInput) {
	ISeg7Init();
	int nowStopwatchDisplayNumber = segmentStopwatchDisplayNumber + clock_counter;
	segmentStopwatchDisplayNumber = nowStopwatchDisplayNumber;
	ISeg7DispNum(nowStopwatchDisplayNumber , 10);
	
	
	switch(stopwatchKeyInput) {
		case SW3:
			LcdPuts("TEST UP");
			break;
			
		case SW7:
			LcdPuts("TEST DOWN");
			break;
	}
	return 1;
}