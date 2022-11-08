/*
 * LCD.h
 *
 * Created: 2022-08-12 오후 3:19:18
 *  Author: kccistc
 */ 


#ifndef LCD_H_
#define LCD_H_

#include "I2C_LCD.h"
#include "button.h"
#include "LED.h"
#include "button.h"
#include <avr/interrupt.h>
#include <stdint.h>

static int power = 0;


void laundry_power(void);
void laundry_ready(void);
void laundry_auto(void);
void laundry_manual(void);
void laundry_running(void);

void init_timer0(void);
void pwm_init(void);

#endif /* LCD_H_ */