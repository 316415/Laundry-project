/*
 * LED.c
 *
 * Created: 2022-08-09 오후 2:39:42
 *  Author: kccistc
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "LED.h"
#include "LCD.h"

int nstate;
int state;
int pre;
int toggle = 0;

static int laundry_state;

int led_command = 0 ; // nothing
//1: ledalltoggle
//2: shift_left_led_on
//3: shift_right_led_on
//4:  
void init_led()
{
	DDRA  = 0xff;
}














void ledalltoggle()
{
	toggle =! toggle;
	if(!toggle)
	 PORTA = 0xff;
	else PORTA = 0x00;
	_delay_ms(300);
}

void shift_left_led_on()
{
	for (int i =0; i<8; i++){
		PORTA = 0b00000001 << i ;
		_delay_ms(300);	
}
}
void shift_right_led_on(void){
	
	for (int i =0; i<8; i++){
		PORTA = 0b10000000 >> i;
		_delay_ms(300);
	}
}
void shift_left_hold_led_on(void)
{
	state = 0b00000001;
	for (int i =0; i<9; i++)
	{
		if(i == 8)
		{
			LED_PORT = 0x00;
		}
		else{
			pre = 0b00000001 << i ;
			nstate = pre | state;
			PORTA = nstate;
			state = nstate;
			_delay_ms(300);
		}
	}
}
void shift_right_hold_led_on(void)
{
	state = 0b10000000;
	for (int i =0; i<9; i++)
	{
		if(i == 8)
		{
			LED_PORT = 0x00;
		}
		else{
			pre = 0b10000000 >> i ;
			nstate = pre | state;
			PORTA = nstate;
			state = nstate;
			_delay_ms(300);
		}
	}
}
void floweroff(void){
	for(int i =0 ; i<4; i++)
	{
		PORTA = (0b00000001<<i ) | (0b10000000 >> i);
		_delay_ms(300);
	}
}
void floweron(void){
	for(int i =0 ; i<4; i++)
	{
		PORTA = (0b00010000<<i ) | (0b00001000 >> i);
		_delay_ms(300);
	}
}
