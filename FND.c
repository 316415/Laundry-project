/*
 * FND.c
 *
 * Created: 2022-08-16 오전 9:28:27
 *  Author: kccistc
 */ 

#include "button.h"

extern int ms_cnt;
extern int sec_cnt;

int digit_position = 0;
int rec_cnt = 0;

void display_ready(void)
{
	unsigned char fnd_rectangular[] = {0b11111111, 0b11111110, 0b11111100, 0b11111000, 0b11110000, 0b11110110, 0b11100110, 0b11000110, 0b11011111, 0b11001111, 0b11000111, 0b11110111, 0b11110011, 0b11110001};
	switch(digit_position)
	{
		case 0:
		FND_DIGIT_PORT = 0b10000000;
		FND_DATA_PORT = fnd_rectangular[4];
		break;
		case 1:
		FND_DIGIT_PORT = 0b01000000;
		FND_DATA_PORT = fnd_rectangular[5];
		break;
		case 2:
		FND_DIGIT_PORT = 0b00010000;
		FND_DATA_PORT = fnd_rectangular[5];
		break;
		case 3:
		FND_DIGIT_PORT = 0b00001000;
		FND_DATA_PORT = fnd_rectangular[7];
		break;
		
	}
}

void display_fnd(void)
{
	unsigned char fnd_font[] = {0b11000000, 0b11111001, 0b10100100, 0b10110000, 0b10011001, 0b10010010, 0b10000011, 0b11011000, 0b10000000, 0b10011000};

	switch(digit_position)
	{
		case 0: //1단위
		FND_DIGIT_PORT = 0b10000000;
		if (sec_cnt%2==1)
		{
			FND_DATA_PORT = fnd_font[sec_cnt%10] + 0b10000000; //0~9
		}else
		{
			FND_DATA_PORT = fnd_font[sec_cnt%10];
		}

		break;
		case 1: //10단위
		FND_DIGIT_PORT =0b01000000;
		FND_DATA_PORT = fnd_font[sec_cnt/10%6]; //두 자리 초
		break;
		case 2: //100단위
		FND_DIGIT_PORT =0b00010000;
		FND_DATA_PORT = fnd_font[sec_cnt/60%10];
		break;
		case 3: //1000단위
		FND_DIGIT_PORT =0b00001000;
		FND_DATA_PORT = fnd_font[sec_cnt/600%6];
		break;
	}
}

void display_rectangular(void)
{
	int a, s, t;
	t = sec_cnt % 13;
	
	unsigned char fnd_rectangular[] = {0b11111111, 0b11111110, 0b11111100, 0b11111000, 0b11110000, 0b11110110, 0b11100110, 0b11000110, 0b11011111, 0b11001111, 0b11000111, 0b11110111, 0b11110011, 0b11110001};
	switch(digit_position)
	{
		case 0: //1단위
		FND_DIGIT_PORT = 0b10000000;
		if(t == 12)
		{
			a = 0;
		}
		else if(t >6)
		{
			a = 4;
		}
		else if(t > 2)
		{
			a = t - 2;
		}
		else
		{
			a = 0;
		}
		FND_DATA_PORT = fnd_rectangular[a];
		break;
		case 1: //10단위
		FND_DIGIT_PORT =0b01000000;
		if(t == 12)
		{
			a = 0;
		}
		else if(t >6)
		{
			a= 5;
		}
		else if(t > 1)
		{
			a = 1;
		}
		else
		{
			a = 0;
		}
		FND_DATA_PORT = fnd_rectangular[a];
		break;
		case 2: //100단위
		FND_DIGIT_PORT =0b00010000;
		if(t == 12)
		{
			a = 0;
		}
		else if(t >7)
		{
			a= 5;
		}else if(t > 0)
		{
			a = 1;
		}
		else
		{
			a = 0;
		}
		FND_DATA_PORT = fnd_rectangular[a];
		break;
		case 3: //1000단위
		FND_DIGIT_PORT =0b00001000;
		if(t==12)
		{
			a = 0;
		}else if(t >8)
		{
			a = t - 4;
		}
		else
		{
			a = 1;
		}
		FND_DATA_PORT = fnd_rectangular[a];
		break;
	}
}

void init_fnd(void)
{
	FND_DATA_DDR = 0xff;
	FND_DIGIT_DDR |= 0xd8; //4567만 1로 3210은 0 그대로 유지
	#if 1
	FND_DIGIT_PORT = 0x00; //common anode 기준 all off
	#else
	FND_DIGIT_PORT = ~0x00; //common cathode 기준 all off
	#endif
}