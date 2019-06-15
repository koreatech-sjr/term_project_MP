
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "iseg7.h"			//7-세그먼트 구동 헤더파일

#define N_BUF		6		//수를 저장할 버퍼의 크기
#define N_SEGMENT	4		//7-세그먼트의 개수

#define SSEG_CONT	PORTD   	//7세그먼트 제어 포트
#define DDR_CONT	DDRD
#define SSEG_DATA	PORTA   	//7세그먼트 데이터 포트
#define DDR_DATA	DDRA

#define CONT_MASK    ((1<<PD7)|(1<<PD6)|(1<<PD5)|(1<<PD4))
//제어포트로 사용하는 포트 마스크
#define OUTPUT_VALUE (1<<PD4)      //제어포트 중 최하위 비트로 설정

//====================================================
// 수에 대한 세그먼트의 코드
// 마지막 코드 SegTable[16]은 세그먼트를 끄는 데 사용
//====================================================

static unsigned char SegTable[17] // 7-세그먼트 코드 값
= {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6d, 0x7d, 0x07,
	0x7F, 0x6F, 0x77, 0x7C, 0x58, 0x5E, 0x79, 0x71,
0x00};

// cnumber[0] : 1자리수 저장
// cnumber[1] : 10자리수 저장

static unsigned char cnumber[N_BUF] //자리수별로 수를 저장할 배열
= {16,16,16,16,16, 16};      //초기에 모두 끈다.


void ISeg7Init(void)
{
	SSEG_CONT  |=  CONT_MASK; //모든 7-세그먼트를 끈다.
	DDR_CONT   |=  CONT_MASK; //제어포트의 상위니블을
	//출력으로 설정
	DDR_DATA = 0xFF;     //테이터포트는 모두 출력으로 설정
	
	// 타이머/카운터0 비교 일치 인터럽트 설정
	// 타이머/카운터0는 CTC모드로 동작
	
	TCCR0 = (1<<WGM01);		 //CTC모드/타이머 정지, OC0핀 미사용
	OCR0  = 78;				//5msec마다 비교일치 인터럽트 발생
	TIMSK |= (1<<OCIE0);	//타이머0 비교일치 인터럽트 허용
	
	TCCR0 |= 0x07;		   //분주비 1024로 타이머 시작
}


void ISeg7DispNum(unsigned short num, unsigned short radix)
{
	int  j;
	TIMSK &= ~(1<<OCIE0);	// 타이머0 비교일치 인터럽트 금지

	// cnumber[]에 수 0저장
	// cnumber[0] = 0
	// cnumber[1] ~ cnumber[4] = 16: 세그먼트를 끈다.
	
	
	cnumber[0] = 0;
	for(j=1; j<N_BUF; j++) { cnumber[j] = 16; }
	
	j = 0;    		         //1자리를 가리킴
	while(j<=4)
	{
		cnumber[j++] = num % radix;  //나머지
		num /= radix;                 // 몫
	}
	TIMSK |= (1<<OCIE0);	//타이머0 비교일치 인터럽트 허용
}

//====================================================
// ISR : 인터럽트가 걸릴 때마다 index가 가리키는 세그먼트에
//       데이터를 표시하고, index는 계속 회전이동하게 된다.
//=====================================================

ISR(TIMER0_COMP_vect){	//카운터/타이머0 비교일치 인터럽트
	static int index = 0;
		
	// 현재 인덱스가 가리키는 자리수를 표시한다.
	SSEG_CONT = (SSEG_CONT & ~CONT_MASK) | (~(OUTPUT_VALUE<<index) & CONT_MASK);
	SSEG_DATA = ~SegTable[cnumber[index]];
	
	index++; //인덱스 이동
	if(index == N_SEGMENT) { index = 0; }
}
