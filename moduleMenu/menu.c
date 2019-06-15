/*
 * menu.c
 *
 * Created: 2019-06-14 오전 12:41:47
 *  Author: kadam
 */ 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../lib/lcd.h"
#include "../lib/keypad.h"
#include "./menu.h"

static char Set[] = "1. Set         ";
static char Timer[] = "2. Timer         ";
static char StopWatch[] = "3. Stop Watch  ";
static char Alarm[] = "4. Alarm          ";
static char str1[] = "00h 00m 00s";
static int nowMenuCursor = 0;
static int nowStatus = 1;

void initMenu() {
	LcdMove(0,0);
	LcdPuts(str1);
	LcdMove(1,0);
	LcdPuts(Set);
}



// MENU 세팅용 함수
int setMenu(char clickedButton) {
	LcdMove(1,0);			// 2행으로
	switch(clickedButton) {
		// up button clicked
		case SW3:
			if ( nowMenuCursor == 1) {
				nowMenuCursor--;
				LcdPuts(Set);
				nowStatus = 1;
			}
			else if ( nowMenuCursor == 2) {
				nowMenuCursor--;
				LcdPuts(Timer);
				nowStatus = 2;
			}
			else if ( nowMenuCursor == 3) {
				nowMenuCursor--;
				LcdPuts(StopWatch);
				nowStatus = 3;
			}
			else{
				LcdPuts(Set);
				nowStatus = 1;
			}
			return 0;
			break;
		
		
		// down button clicked
		case SW7:
			if ( nowMenuCursor == 0) {
				nowMenuCursor++;
				LcdPuts(Timer);
				nowStatus = 2;
			}
			else if ( nowMenuCursor == 1) {
				nowMenuCursor++;
				LcdPuts(StopWatch);
				nowStatus = 3;
			}
			else if ( nowMenuCursor == 2) {
				nowMenuCursor++;
				LcdPuts(Alarm);
				nowStatus = 4;
			}
			else{
				LcdPuts(Alarm);
				nowStatus = 4;
			}
			return 0;
			break;
		
		
		// When select button clicked, 메인파일의 각 status에 해당하는 기능을 위한 while문을 돌림
		case SW11:
			return nowStatus;
			break;
		
		case SW15:
			return -99;
			break;
			
		default:
			return 0;
			break;
	}
}