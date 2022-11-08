/*
 * prj_laundry_machine.c
 *
 * Created: 2022-08-12 오후 2:05:56
 * Author : kccistc
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "button.h"
#include "I2C.h"
#include "I2C_LCD.h"
#include "LCD.h"

#define SO_02_H 602
#define LA_02   568
#define BEAT_1_4	341
#define BEAT_1_16	86
#define BEAT_1_32	42

extern void button_alarm(void);
extern void Music_Player(int *tone, int *Beats);
extern void init_speaker(void);
extern void Beep(int  repeat);
extern void Siren(int repeat);
extern void RRR(void);

extern const int Elise_Tune[];
extern const int Elise_Beats[];
extern const int GrandFa_Tune[];
extern const int GrandFa_Beats[];

extern void display_ready(void);

int main(void)
{
    init_button();
	I2C_LCD_init();
	init_timer0();
	pwm_init();
	init_laundry();
	init_speaker();
	init_fnd();
	
	sei();
	
    while (1) 
    {
		display_ready();
		laundry_door();
		laundry_paused();
		laundry_ready();
		laundry_auto();
		laundry_manual();
		laundry_running();
		laundry_return();
		laundry_end();
    }
}