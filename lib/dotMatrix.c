/*
 * dotMatrix.c
 *
 * Created: 2019-06-18 오후 5:44:20
 *  Author: pggvn
 */ 
 
 #include <avr/io.h>w
 #include <util/delay.h>
 #include <avr/interrupt.h>
 #include "./dotMatrix.h"
 #include "../lib/iseg7.h"

static int rowindex = 0;		// 행(portE) 점등 변수
static int columnindex = 0;		// 열(portB) 점등 변수
// 도트매트릭스 패턴
static unsigned char dotpattern[4] = {0xFC, 0xF3, 0xCF, 0x3F};
// 도트매트릭스 패턴 변화용 타이머/카운터1 진입 횟수 변수
static int timerDotCounter = 0;
static int timerDotCounterActiveFlag = 0;

 void timer1_init()
 {
	 // set up timer with prescaler = 8
	 TCCR1B |= (1 << CS11);
	 
	 // initialize counter
	 TCNT1 = 0;
	 
	 // enable overflow interrupt
	 TIMSK |= (1 << TOIE1);
	 
	 // enable global interrupts
	 sei();
	 
	 // initialize overflow counter variable
	 timerDotCounter = 0;

	 timerDotCounterActiveFlag = 0;
 }

 void dotmatrixActive(){
	 DDRE = 0xFF;
	 DDRB = 0xFF;
	 
	 timerDotCounterActiveFlag = 1;
	 // 패턴 초기화
	 rowindex = 0;
	 columnindex = 0;
	 PORTE = dotpattern[rowindex];
	 PORTB = 0xFF;

	 while(timerDotCounterActiveFlag == 1){

		 if(++columnindex == 8)	columnindex = 0;
		 PORTE = dotpattern[rowindex];	// 타이머카운터에 의해 패턴 바뀜
		 PORTB = 0x01 << columnindex;
		 
		 //second++;
		 if(rowindex == 4){
			 PORTE = 0xFF;
			 timerDotCounterActiveFlag = 0;
			 break;
		 }
	 }
 }

 ISR(TIMER1_OVF_vect)
 {
	 // keep a track of number of overflows
	 timerDotCounter++;
	 
	 // check for number of overflows here itself
	 // 30 overflows = 1 seconds delay (approx.)
	 if (timerDotCounter >= 30) // NOTE: '>=' used instead of '=='
	 {
		//rowindex+=1;

		 if(rowindex++ == 5) {
			rowindex = 0;
		 }
		 if(timerDotCounterActiveFlag == 0) {
		 	rowindex = 0;
		 }
		 
		 timerDotCounter = 0;   // reset overflow counter
	 }
 }