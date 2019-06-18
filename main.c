#include <avr/io.h>			
#include <avr/interrupt.h>
#include <util./delay.h>	
#include <stdlib.h>
#include "./lib/lcd.h"		
#include "./lib/keypad.h"
#include "./moduleMenu/menu.h"
#include "./moduleClock/clock.h"
#include "./moduleTimer/timer.h"
#include "./moduleStopwatch/stopwatch.h"
#include "./moduleAlarm/alarm.h"

static unsigned char keyInput;

char* startTermProject();	

/*************************************/
/* Clock Counter Settings            */
/*************************************/

// Clock Counter interrupt 진입횟수
static int ClockCounter_CNT = 0; 
// Clock Counter interrupt 으로 계산된 초
static int ClockCounter_SECOND = 0; 
static int ClockCounter_M_SECOND = 0; 
static int ClockCounter_M_SECOND_ONLY_STOPWATCH = 0; 


static int SegmentTimerInitFlag = 0;
static int SegmentStopwatchInitFlag = 0;

static int* AlarmTimeStore[3] = {-1,};


void ClockCounterInit(void) {
	TCCR2 |= (1<<WGM01);	//CTC mode
	OCR2 = 100;				//50usec
	TIMSK = (1<<OCIE2);
}

ISR(TIMER2_COMP_vect) {
	ClockCounter_CNT++;
	
	if(ClockCounter_CNT == 20000){	// 5usec X 20000 = 1sec
		ClockCounter_CNT=0;
		ClockCounter_SECOND++;
	}
	if(ClockCounter_CNT % 2000 == 0) {
		ClockCounter_M_SECOND = 1;
		ClockCounter_M_SECOND_ONLY_STOPWATCH += 1;
	}
}

/*************************************/
/* Clock Counter Settings 끝         */
/*************************************/






// 현재 메뉴를 알려주는 변수
static int status = 0;

int main(void) {
back:
	LcdInit();
	KeyInit();
	ClockCounterInit();

	// 박가경's project <깜박 x 3>
	startTermProject();
	initMenu();
		
	// 전역 인터럽트 허용
	sei();						
	TCCR2 |= 1<<CS21;
	// 전역 인터럽트 허용 끝
		
	
	while(1){
		// 계속해서 클락 세팅 sec parameter는 디스플레이 플레그 아님 

		setClock(ClockCounter_SECOND, SegmentStopwatchInitFlag, AlarmTimeStore);

		if(SegmentTimerInitFlag == 1) {
			SegmentTimerInitFlag = set7SegmentTimer(ClockCounter_M_SECOND);
		}
		
		if(SegmentStopwatchInitFlag == 1) {
			SegmentStopwatchInitFlag = set7SegmentStopwatch(ClockCounter_M_SECOND, KeyInput());
			ClockCounter_M_SECOND = 0;
			continue;
		}
		
		// 무조건 1초 혹은 0.1초로 계산되게 하기위함
		ClockCounter_SECOND = 0;
		ClockCounter_M_SECOND = 0;		
		// keypad input
		keyInput = KeyInput();
		
		// 어떤 상태에서 누르느냐에 따라 실행되는게 다름
		status = setMenu(keyInput);	

		//==========================================================
		//1. SET 에 해당됨
		while(status==1) {								
			status = setCustomClock(&ClockCounter_SECOND);	// Time setting(main.c 파일 안에 있음)	
		}
		
	
		//=========================================================
		//status = 2
		while(status==2){
			status = setTimer(&SegmentTimerInitFlag);
		}
		
		
		//=========================================================
		//status = 3
		while(status==3){
			status = setStopwatch(&SegmentStopwatchInitFlag);
		}
	
		
		//=========================================================
		//status = 4
		while(status==4){
			status = setAlarm();
			int* tmp = getAlarmTime();
			
			AlarmTimeStore[0] = tmp[0];
			AlarmTimeStore[1] = tmp[1];
			AlarmTimeStore[2] = tmp[2];
		}
		
		// 이전 버튼
		if(status == -99) {
			goto back;
		}
		
	/*	// select key pressed   >>   선택버튼 눌렸을 때 setMenu에서 nowStatus 값을 받아 1번일때 case1
		switch(keyInput) {
			case SW11:
			switch(nowStatus) {
				// function control by nowStatus
				case 1:
				// TODO: 시간 세팅에 대한 메소드 호출
				nowStatusActivateFlag = 0;	// nowStatus CHANGE X
				
				while(1) {
					keyInput = KeyInput();
					nowStatusActivateFlag = 0;
					// if(nowStatusActivateFlag == 0) get "inputData"
					nowInputKeypad = nowStatusActivateFlag != 1 ? inputData(keyInput) : -1;
					
					char nowInputKeypad2;
					nowInputKeypad2 = inputData(nowInputKeypad);	// Protect a typing error
					setTime(nowInputKeypad2);	// Time setting
					
					// 다시 set 눌릴 경우 빠져나오기
					if(keyInput == SW11){
						nowStatusActivateFlag = 1;	// nowStatus CHANGE O
						break;
					}
				}
				
				break;
				
				case 2:
				//
				break;
				
				case 3:
				break;
				
				case 4:
				break;
			}
			break;
			
		}*/
	}
}


//============================================================================
//======박가경's Watch===flash x 3==============================================
//============================================================================
char* startTermProject() {
	char *initTitle = malloc(sizeof(char) * 20);
	
	char font1[8] = {0x02, 0x1A, 0x1F, 0x1E, 0x02, 0x1C, 0x04, 0x04};	// 박
	char font2[8] = {0x02, 0x02, 0x1A, 0x0B, 0x0A, 0x0A, 0x02, 0x00};	// 가
	char font3[8] = {0x02, 0x1E, 0x0A, 0x0E, 0x08, 0x0C, 0x12, 0x0C};	// 경
	
	LcdNewchar(1, font1);
	LcdNewchar(2, font2);
	LcdNewchar(3, font3);

	initTitle[0] = 1;
	initTitle[1] = 2;
	initTitle[2] = 3;
	initTitle[3] = '\'';
	initTitle[4] = 's';
	initTitle[5] = ' ';
	initTitle[6] = 'W';
	initTitle[7] = 'a';
	initTitle[8] = 't';
	initTitle[9] = 'c';
	initTitle[10] = 'h';
	initTitle[11] = '\0';
	
	LcdMove(0,0);			// cursor go to home
	LcdPuts(initTitle);		// 박가경's Watch
	
	for(int i=0; i<3; i++)		// flash x 3
	{
		LcdCommand(DISP_ON);		// 점
		msec_delay(500);			// 0.5s delay
		LcdCommand(DISP_OFF);		// 멸
		msec_delay(500);			// 0.5s delay
	}
	LcdInit();
}

/*
int setTimer() {
	int x=0;
	int num=0;
	for(x=0; x<4; x++){
		realTimer[x]='0';
	}
	keyInput = NO_KEY;	//키가 눌리지 않으면
	keyInput = KeyInput();
	switch(keyInput) {
		case SW3:	//화살표 위로 누르면 커서 왼쪽 이동
			LcdCommand(CURSOR_LSHIFT);
			case SW7:	//화살표 아래 누르면 커서 오른쪽 이동
			LcdCommand(CURSOR_RSHIFT);
		
		case SW0:
			realTimer[num]='1';		//배열에 1이 저장
			LcdPutchar('1');
			num++; 			//키가 눌려서 배열 한 칸이 차면 다음 칸으로!
			return 2;
			break;
		
		case SW1:				//위와 같음
			realTimer[num]='2';
			LcdPutchar('2');
			num++;
			return 2;
			break;
			
		case SW2:
			realTimer[num]='3';
			LcdPutchar('3');
			num++;
			return 2;
			break;
		
		case SW4:
			realTimer[num]='4';
			LcdPutchar('4');
			num++;
			return 2;
			break;
			
		case SW5:
			realTimer[num]='5';
			LcdPutchar('5');
			num++;
			return 2;
			break;
		
		case SW6:
			realTimer[num]='6';
			LcdPutchar('6');
			num++;
			return 2;
			break;
		
		case SW8:
			realTimer[num]='7';
			LcdPutchar('7');
			num++;
			return 2;
			break;
		
		case SW9:
			realTimer[num]='8';
			LcdPutchar('8');
			num++;
			return 2;
			break;
		
		case SW10:
			realTimer[num]='9';
			LcdPutchar('9');
			num++;
			return 2;
			break;
		
		case SW13:
			realTimer[num]='0';
			LcdPutchar('0');
			num++;
			return 2;
			break;
		
		case SW11:
			LcdMove(1,0);
			LcdPuts(Timer);
			minTimer = realTimer[0]-'0';
			stenTimer = realTimer[1]-'0';
			soneTimer = realTimer[2]-'0';
			psecTimer = realTimer[3]-'0';
			timerData = minTimer*1000 + stenTimer*100 + soneTimer*10 + psecTimer;
			return 0;
		
		default:
			break;
	}
	
} */

