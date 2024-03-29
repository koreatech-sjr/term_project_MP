﻿/*
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
static int segmentStopwatchDisplayNumberArray[1000] = {0,};
// stopwatch 기능 선택시 초기 화면 세팅용 플래그 ; 0이면 초기 진입
static int customStopwatchSetInitFlag = 0;
// stopwatch stop flag
static int stopwatchStopFlag = 0;

static int stopwatchDisplayIndex = 0;

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
			case SW15:
				return -99;
				break;
			default:
				returnStatus = 3;
				break;
		}
		// 확인이 눌린 경우
		if(returnStatus == 0) {
			
		}
		return returnStatus;
	}
}

// 스톱워치 시간증가 시작 및 버튼 별 기능 구현
int set7SegmentStopwatch(int clock_counter, unsigned char stopwatchKeyInput) {
	ISeg7Init();
	if(stopwatchStopFlag == 0 ) {	
		int nowStopwatchDisplayNumber = segmentStopwatchDisplayNumber + clock_counter;
		if(nowStopwatchDisplayNumber == 600) {	
			nowStopwatchDisplayNumber += 400;	// 60초가 되면 1분으로 갱신
		}
		segmentStopwatchDisplayNumber = nowStopwatchDisplayNumber;
		ISeg7DispNum(nowStopwatchDisplayNumber , 10);	
	}
	
	switch(stopwatchKeyInput) {
		case SW3:
			// 스톱워치 진행중 시간기록
			if(stopwatchStopFlag == 0){
			
				// 시간이 기록됨을 알리는 모터구동**************
				
				DDRG = 0xFF;
				PORTG = 0x0F;
				msec_delay(100);   //0.1초 동안 구동
				PORTG = 0x00;
								
				//************************************************


				
				// 첫 or 두번째 입력
				if(stopwatchDisplayIndex<=1) {
					LcdMove(stopwatchDisplayIndex, 0);
					segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex] = segmentStopwatchDisplayNumber;
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex]/1000+'0');	//1000으로 나누면 몫은 '분'에 해당하는 숫자만 남음.
					LcdPutchar('m');
					LcdPutchar(' ');
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex]%1000/100+'0');	//1000으로 나눈 나머지를 100으로 나누면 초의 십의자리에 해당하는 숫자만 남음
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex]%100/10+'0'); //위와 비슷한 논리
					LcdPutchar('.');
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex]%10+'0'); //위와 비슷한 논리
				}
				// 세번째 이상 입력
				else {
					segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex] = segmentStopwatchDisplayNumber;
					// 0번째 행 변경
					LcdMove(0,0);
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex-1]/1000+'0');
					LcdPutchar('m');
					LcdPutchar(' ');
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex-1]%1000/100+'0');
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex-1]%100/10+'0');
					LcdPutchar('.');
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex-1]%10+'0');
					
					// 1번째 행 변경
					LcdMove(1,0);
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex]/1000+'0');
					LcdPutchar('m');
					LcdPutchar(' ');
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex]%1000/100+'0');
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex]%100/10+'0');
					LcdPutchar('.');
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex]%10+'0');
				}
				stopwatchDisplayIndex += 1;
			}
			// 아래키 눌러서 stopwatchStopFlag = 1이 된 경우 즉, 스톱워치가 멈춘 경우
			else {
				// 기록된 시간이 3개 이상인 경우에만
				if(stopwatchDisplayIndex >= 2){				
					LcdMove(0,0);
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex-2]/1000+'0');
					LcdPutchar('m');
					LcdPutchar(' ');
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex-2]%1000/100+'0');
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex-2]%100/10+'0');
					LcdPutchar('.');
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex-2]%10+'0');
					
					// 1번째 행 변경
					LcdMove(1,0);
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex-1]/1000+'0');
					LcdPutchar('m');
					LcdPutchar(' ');
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex-1]%1000/100+'0');
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex-1]%100/10+'0');
					LcdPutchar('.');
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex-1]%10+'0');	
					stopwatchDisplayIndex -= 1;
				}
			}
			break;
			
		case SW7:
			if(stopwatchStopFlag == 1) {	//스톱워치가 멈춘 경우		
				if(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex+1] != 0){
					LcdMove(0,0);
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex]/1000+'0');
					LcdPutchar('m');
					LcdPutchar(' ');
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex]%1000/100+'0');
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex]%100/10+'0');
					LcdPutchar('.');
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex]%10+'0');
					
					// 1번째 행 변경
					LcdMove(1,0);
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex+1]/1000+'0');
					LcdPutchar('m');
					LcdPutchar(' ');
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex+1]%1000/100+'0');
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex+1]%100/10+'0');
					LcdPutchar('.');
					LcdPutchar(segmentStopwatchDisplayNumberArray[stopwatchDisplayIndex+1]%10+'0');
					stopwatchDisplayIndex += 1;
				}
			}
			else {
				stopwatchStopFlag = 1;
				stopwatchDisplayIndex-=1;	
			}
			break;
		case SW15:
			return -99;
			break;
	}
	return 1;
}