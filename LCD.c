/*
 * LCD.c
 *
 * Created: 2022-08-12 오후 3:18:56
 *  Author: kccistc
 */ 

#include "LCD.h"

int i;
static int laundry_state = 0;
int start_state = 0;
int mode_state = 0;
int auto_state = 0;
int manual_state = 0;
int paused_state = 0;
int music_state = 0;
int paused_temp;
int wash = 0;
int wash_time = 0;
int rinse = 0;
int rinse_time = 0;
int dry = 0;
int dry_time = 0;
int end_state = 0;
char laundry_message_auto0[50] = "* Auto Mode     ";
char laundry_message_auto1[50] = "  Auto Mode     ";
char laundry_message_manual0[50] = "* Manual Mode   ";
char laundry_message_manual1[50] = "  Manual Mode   ";
char laundry_auto0[50] = "  strong     ";
char laundry_auto1[50] = "  normal     ";
char laundry_auto2[50] = "  weak       ";
char laundry_manual1[50] = "             ";
char laundry_manual2[50] = "dry:0        ";
char wash_timer[50];
char rinse_timer[50];
char dry_timer[50];


#define SO_02_H 602
#define LA_02   568
#define BEAT_1_16	86

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

extern void display_rectangular(void);
extern void display_fnd(void);
extern void displya_ready(void);

int ms_cnt = 0;
int sec_cnt = 0;
int digit_cnt = 0;

extern int digit_position;

ISR(TIMER0_OVF_vect)
{
	TCNT0=6;
	ms_cnt++;
	digit_cnt++;
	if (ms_cnt >= 1000)
	{
		ms_cnt = 0;
		sec_cnt++;
	}
	digit_position = digit_cnt % 4;
	if (digit_cnt >= 1000)
	{
		digit_cnt = 0;
	}
}

void pwm_init()
{
	// 모드5 : 8비트고속PWM모드 timer 3
	TCCR3A |= (1 << WGM30);
	TCCR3B |= (1 << WGM32);
	
	// 비반전 모드 TOP: 0xff 비교일치값 : OCR3C : PE5
	TCCR3A |= (1 << COM3C1);
	// 분주비 64  16000000/64  ==> 250000HZ(250KHZ)
	// 256 / 250000 ==> 1.02ms
	// 127 / 250000 ==> 0.5ms
	TCCR3B |= ( 1 << CS31)  | (1 << CS30);   // 분주비 64
	// 1600000HZ / 64분주 ==> 250000HZ (250KHZ)
	//
	OCR3C=0;  // PORTE.5
	
	DDRE |= ( 1 << 0) | ( 1 << 2) | ( 1 << 5);
}

void init_laundry(void)
{
		laundry_state = 0;
		start_state = 0;
		mode_state = 0;
		auto_state = 0;
		manual_state = 0;
		paused_state = 0;
		music_state = 0;
		paused_temp;
		wash = 0;
		wash_time = 0;
		rinse = 0;
		rinse_time = 0;
		dry = 0;
		dry_time = 0;
		end_state = 0;
		DDRA  = 0xff;
}

void laundry_door(void)
{
	if (laundry_state == 0 && power == 0)
	{
		if (get_button4())
		{
			button_alarm();
			display_ready();
			power = 1;
			PORTA = 0x01;
			I2C_LCD_write_string_XY(0,0,"Open Door  ");
			I2C_LCD_write_string_XY(1,0,"                ");
		}
	}
	else if (laundry_state == 0 && power == 1)
	{
		if (get_button4())
		{
			button_alarm();
			display_ready();
			power = 0;
			PORTA = 0x00;
			I2C_LCD_write_string_XY(0,0,"Closed Door");
			I2C_LCD_write_string_XY(1,0,"                ");
		}
	}
}

void laundry_ready(void)
{
	if (power == 0 && laundry_state == 0 && get_button1())
	{
		button_alarm();
		PORTA = 0x02;
		laundry_state = 1;
		mode_state = 1;
		I2C_LCD_write_string_XY(0,0,laundry_message_auto0);
		I2C_LCD_write_string_XY(1,0,laundry_message_manual1);
	}
	if (laundry_state == 1)
	{
		if (mode_state == 0 && get_button1())
		{
			button_alarm();
			PORTA = 0x02;
			mode_state = 1;
			I2C_LCD_write_string_XY(0,0,laundry_message_auto0);
			I2C_LCD_write_string_XY(1,0,laundry_message_manual1);
		}
		else if (mode_state == 1 && get_button1())
		{
			button_alarm();
			PORTA = 0x02;
			mode_state = 0;
			I2C_LCD_write_string_XY(0,0,laundry_message_auto1);
			I2C_LCD_write_string_XY(1,0,laundry_message_manual0);
		}
	}
}

void laundry_auto(void)
{
	if (mode_state == 1)
	{
		if (get_button2())
		{
			button_alarm();
			PORTA = 0x02;
			paused_state = 0;
			laundry_state = 2;
			auto_state++;
			if (auto_state > 2)
			{
				auto_state = 0;
			}
			if (auto_state == 2)
			{
				I2C_LCD_write_string_XY(0,0,laundry_message_auto0);
				I2C_LCD_write_string_XY(1,0,laundry_auto2);
				wash = 1;
				rinse = 2;
				dry = 2;
			}
			else if (auto_state == 1)
			{
				I2C_LCD_write_string_XY(1,0,laundry_auto1);
				wash = 3;
				rinse = 2;
				dry = 2;
			}
			else
			{
				I2C_LCD_write_string_XY(1,0,laundry_auto0);
				wash = 5;
				rinse = 2;
				dry = 2;
			}
		}
	}
}

void laundry_manual(void)
{
	if (mode_state == 0)
	{
		if (get_button2())
		{
			button_alarm();
			PORTA = 0x02;
			paused_state = 0;
			laundry_state = 3;
			manual_state++;
			if (manual_state > 2)
			{
				manual_state = 0;
				sprintf(laundry_manual1,"*wash:%d  rinse:%d",wash, rinse);
				sprintf(laundry_manual2," dry:%d        ",dry);
				I2C_LCD_write_string_XY(0,0,laundry_manual1);
				I2C_LCD_write_string_XY(1,0,laundry_manual2);
			}
		}
		if (end_state == 0 && manual_state == 0 && paused_state == 0)
		{
			if (laundry_state == 3 && get_button1())
			{
				button_alarm();
				PORTA = 0x02;
				wash++;
				if (wash > 5)
				{
					wash = 0;
				}
				sprintf(laundry_manual1,"*wash:%d  rinse:%d",wash, rinse);
				sprintf(laundry_manual2," dry:%d        ",dry);
				I2C_LCD_write_string_XY(0,0,laundry_manual1);
				I2C_LCD_write_string_XY(1,0,laundry_manual2);
			}
		}
		else if (end_state == 0 && manual_state == 1 && paused_state == 0)
		{
			if (laundry_state == 3 && get_button1())
			{
				button_alarm();
				PORTA = 0x02;
				rinse++;
				if (rinse > 5)
				{
					rinse = 0;
				}
			}
			sprintf(laundry_manual1," wash:%d *rinse:%d",wash, rinse);
			sprintf(laundry_manual2," dry:%d        ",dry);
			I2C_LCD_write_string_XY(0,0,laundry_manual1);
			I2C_LCD_write_string_XY(1,0,laundry_manual2);
		}
		else if (end_state == 0 && manual_state == 2 && paused_state == 0)
		{
			if (laundry_state == 3 && get_button1())
			{
				button_alarm();
				PORTA = 0x02;
				dry++;
				if (dry > 5)
				{
					dry = 0;
				}
			}
			sprintf(laundry_manual1," wash:%d  rinse:%d",wash, rinse);
			sprintf(laundry_manual2,"*dry:%d  ",dry);
			I2C_LCD_write_string_XY(0,0,laundry_manual1);
			I2C_LCD_write_string_XY(1,0,laundry_manual2);
		}
	}
}

void laundry_running(void)
{
	if (laundry_state == 2 && get_button3())
	{
		button_alarm();
		PORTA = 0x04;
		sec_cnt = 0;
		wash_time = wash *16;
		rinse_time = rinse * 10;
		dry_time = dry * 10;
		laundry_state = 4;
	}
	if (end_state == 0 && laundry_state == 3 && get_button3())
	{
		button_alarm();
		PORTA = 0x04;
		sec_cnt = 0;
		wash_time = wash *16;
		rinse_time = rinse * 10;
		dry_time = dry * 10;
		laundry_state = 4;
	}
	if (laundry_state == 4 && wash_time == 0 && rinse_time == 0 && dry_time == 0)
	{
		end_state = 1;
	}
	
	if (end_state == 0 && laundry_state == 4 && (wash_time -sec_cnt) >= 0 && wash_time >= 0)
	{
		for(;sec_cnt<wash_time;)
		{
			if (get_button4())
			{
				button_alarm();
				paused_temp = sec_cnt-1;
				while (1)
				{
					display_ready();
					PORTA = 0x01;
					I2C_LCD_write_string_XY(0,0,"paused          ");
					I2C_LCD_write_string_XY(1,0,"by user!!       ");
					OCR3C=0;
					if (get_button4())
					{
						button_alarm();
						break;
					}
				}
				sec_cnt = paused_temp;
			}
			display_rectangular();
			PORTA = 0x04;
			I2C_LCD_write_string_XY(0,0,"Washing now...  ");
			sprintf(wash_timer, "left time : %2d s", wash_time-sec_cnt);
			I2C_LCD_write_string_XY(1,0,wash_timer);
			switch(sec_cnt % 16)
			{
				case 0:
				OCR3C=0;
				PORTE |= 0b00000101;
				break;
				case 1:
				PORTE &= 0b11111011;  // 모터정지 : PE0:1 PE2:0
				PORTE |= 0b00000001;
				OCR3C=127;
				case 2:
				PORTE &= 0b11111011;
				PORTE |= 0b00000001;
				OCR3C=127;
				break;
				case 3:
				PORTE &= 0b11111011;
				PORTE |= 0b00000001;
				OCR3C=255; 
				case 4:
				PORTE &= 0b11111011;
				PORTE |= 0b00000001;
				OCR3C=255;
				case 5:
				PORTE &= 0b11111011;
				PORTE |= 0b00000001;
				OCR3C=255;
				case 6:
				PORTE &= 0b11111011;
				PORTE |= 0b00000001;
				OCR3C=255;
				case 7:
				PORTE &= 0b11111011;
				PORTE |= 0b00000001;
				OCR3C=255;
				break;
				case 8:
				OCR3C=0;
				PORTE |= 0b00000101;
				break;
				case 9:
				PORTE &= 0b11111110;
				PORTE |= 0b00000100;
				OCR3C=127;
				case 10:
				PORTE &= 0b11111110;
				PORTE |= 0b00000100;
				OCR3C=127;
				break;
				case 11:
				PORTE &= 0b11111110;
				PORTE |= 0b00000100;
				OCR3C=255;
				case 12:
				PORTE &= 0b11111110;
				PORTE |= 0b00000100;
				OCR3C=255;
				case 13:
				PORTE &= 0b11111110;
				PORTE |= 0b00000100;
				OCR3C=255;
				case 14:
				PORTE &= 0b11111110;
				PORTE |= 0b00000100;
				OCR3C=255;
				case 15:
				PORTE &= 0b11111110;
				PORTE |= 0b00000100;
				OCR3C=255;
				break;
			}
		}
		wash_time = -1;
	}
	else if (laundry_state == 4 && rinse_time >= dry_time && rinse_time > 1 && dry_time > -1)
	{
		sec_cnt = 0;
		for (;sec_cnt < 10;)
		{
			if (get_button4())
			{
				button_alarm();
				PORTA = 0x01;
				paused_temp = sec_cnt-1;
				while (1)
				{
					display_ready();
					I2C_LCD_write_string_XY(0,0,"paused          ");
					I2C_LCD_write_string_XY(1,0,"by user!!       ");
					OCR3C=0;
					if (get_button4())
					{
						button_alarm();
						break;
					}
				}
				sec_cnt = paused_temp;
			}
			PORTA = 0x04;
			display_fnd();
			I2C_LCD_write_string_XY(0,0,"Rinsing now...  ");
			sprintf(rinse_timer, "left time : %2d s", 10-sec_cnt);
			I2C_LCD_write_string_XY(1,0,rinse_timer);
			switch(sec_cnt % 10)
			{
				case 0:
				OCR3C=0;
				PORTE |= 0b00000101;
				break;
				case 1:
				PORTE &= 0b11111011;  // 모터정지 : PE0:1 PE2:0
				PORTE |= 0b00000001;
				OCR3C=127;
				break;
				case 2:
				PORTE &= 0b11111011;
				PORTE |= 0b00000001;
				OCR3C=255;
				case 3:
				PORTE &= 0b11111011;
				PORTE |= 0b00000001;
				OCR3C=255;
				case 4:
				PORTE &= 0b11111011;
				PORTE |= 0b00000001;
				OCR3C=255;
				break;
				case 5:
				OCR3C=0;
				PORTE |= 0b00000101;
				break;
				case 6:
				PORTE &= 0b11111110;
				PORTE |= 0b00000100;
				OCR3C=127;
				break;
				case 7:
				PORTE &= 0b11111110;
				PORTE |= 0b00000100;
				OCR3C=255;
				case 8:
				PORTE &= 0b11111110;
				PORTE |= 0b00000100;
				OCR3C=255;
				case 9:
				PORTE &= 0b11111110;
				PORTE |= 0b00000100;
				OCR3C=255;
				break;
			}
		}
		rinse_time -= 10;
	}
	else if (laundry_state == 4 && rinse_time < dry_time && dry_time >= 0 && rinse_time >= -10)
	{
		sec_cnt = 0;
		for (;sec_cnt < 10;)
		{
			if (get_button4())
			{
				button_alarm();
				paused_temp = sec_cnt-1;
				while (1)
				{
					display_ready();
					PORTA = 0x01;
					I2C_LCD_write_string_XY(0,0,"paused          ");
					I2C_LCD_write_string_XY(1,0,"by user!!       ");
					OCR3C=0;
					if (get_button4())
					{
						button_alarm();
						break;
					}
				}
				sec_cnt = paused_temp;
			}
			display_fnd();
			PORTA = 0x04;
			I2C_LCD_write_string_XY(0,0,"Drying now...   ");
			sprintf(dry_timer, "left time : %2d s", 10-sec_cnt);
			I2C_LCD_write_string_XY(1,0,dry_timer);
			PORTE &= 0b11111110;
			PORTE |= 0b00000100;
			OCR3C=255;
		}
		dry_time -= 10;
	}
	else if (laundry_state == 4 && wash_time <= 0 && rinse_time <= 0 && dry_time <= 0)
	{
		OCR3C=0;
		end_state = 1;
		laundry_state = 4;
	}
	
	if (end_state == 1)
	{
		PORTA = 0x02;
		I2C_LCD_write_string_XY(0,0,"laundry         ");
		I2C_LCD_write_string_XY(1,0,"finished        ");
		if (music_state == 0)
		{
			music_state= 1;
			Music_Player(Elise_Tune,Elise_Beats);
		}
	}
}

void laundry_return(void)
{
	if (laundry_state == 4)
	{
		if (get_button1())
		{
			PORTA = 0x02;
			button_alarm();
			laundry_state = 1;
			mode_state = 0;
			auto_state = 0;
			manual_state = 0;
			paused_state = 0;
			music_state = 0;
			wash = 0;
			wash_time = 0;
			rinse = 0;
			rinse_time = 0;
			dry = 0;
			dry_time = 0;
			end_state = 0;
			I2C_LCD_write_string_XY(0,0,laundry_message_auto1);
			I2C_LCD_write_string_XY(1,0,laundry_message_manual0);
		}
	}
	if (laundry_state == 1)
	{
		if (get_button1())
		{
			button_alarm();
			PORTA = 0x02;
			mode_state = !mode_state;
			if (mode_state == 0)
			{
				I2C_LCD_write_string_XY(0,0,laundry_message_auto1);
				I2C_LCD_write_string_XY(1,0,laundry_message_manual0);
			}
			else if (mode_state == 1)
			{
				I2C_LCD_write_string_XY(0,0,laundry_message_auto0);
				I2C_LCD_write_string_XY(1,0,laundry_message_manual1);
			}
		}
	}
}

void laundry_paused(void)
{
	if (laundry_state == 2)
	{
		if (auto_state == 0)
		{
			if (laundry_state == 2 && paused_state == 0 && get_button4())
			{
				button_alarm();
				PORTA = 0x01;
				display_ready();
				paused_state = 1;
				I2C_LCD_write_string_XY(0,0,"paused          ");
				I2C_LCD_write_string_XY(1,0,"by user!!       ");
			}
			else if (laundry_state == 2 && paused_state == 1 && get_button4())
			{
				button_alarm();
				PORTA = 0x01;
				display_ready();
				paused_state = 0;
				I2C_LCD_write_string_XY(0,0,laundry_message_auto0);
				I2C_LCD_write_string_XY(1,0,laundry_auto0);
				wash = 5;
				rinse = 2;
				dry = 2;
			}
		}
		else if (auto_state == 1)
		{
			if (paused_state == 0 && get_button4())
			{
				button_alarm();
				PORTA = 0x01;
				display_ready();
				paused_state = 1;
				I2C_LCD_write_string_XY(0,0,"paused          ");
				I2C_LCD_write_string_XY(1,0,"by user!!       ");
			}
			else if (paused_state == 1 && get_button4())
			{
				button_alarm();
				PORTA = 0x01;
				display_ready();
				paused_state = 0;
				I2C_LCD_write_string_XY(0,0,laundry_message_auto0);
				I2C_LCD_write_string_XY(1,0,laundry_auto1);
				wash = 3;
				rinse = 2;
				dry = 2;
			}
		}
		else if (auto_state == 2)
		{
			if (paused_state == 0 && get_button4())
			{
				button_alarm();
				PORTA = 0x01;
				display_ready();
				paused_state = 1;
				I2C_LCD_write_string_XY(0,0,"paused          ");
				I2C_LCD_write_string_XY(1,0,"by user!!       ");
			}
			else if (paused_state == 1 && get_button4())
			{
				button_alarm();
				PORTA = 0x01;
				display_ready();
				paused_state = 0;
				I2C_LCD_write_string_XY(0,0,laundry_message_auto0);
				I2C_LCD_write_string_XY(1,0,laundry_auto2);
				wash = 1;
				rinse = 2;
				dry = 2;
			}
		}
	}
	
	if (laundry_state == 3)
	{
		if (paused_state == 0 && get_button4())
		{
			button_alarm();
			PORTA = 0x01;
			display_ready();
			paused_state = 1;
			I2C_LCD_write_string_XY(0,0,"paused          ");
			I2C_LCD_write_string_XY(1,0,"by user!!       ");
		}
		else if (paused_state == 1 && get_button4())
		{
			button_alarm();
			PORTA = 0x01;
			display_ready();
			paused_state = 0;
			sprintf(laundry_manual1,"wash:%d  rinse:%d",wash, rinse);
			sprintf(laundry_manual2,"dry:%d        ",dry);
			I2C_LCD_write_string_XY(0,0,laundry_manual1);
			I2C_LCD_write_string_XY(1,0,laundry_manual2);
		}	
	}
}

void laundry_end(void)
{
	if (laundry_state == 1 || end_state == 1)
	{
		if (get_button4())
		{
			button_alarm();
			PORTA = 0x00;
			display_ready();
			sec_cnt = 0;
			I2C_LCD_write_string_XY(0,0,"laundry off     ");
			I2C_LCD_write_string_XY(1,0,"                ");
			laundry_state = 0;
			mode_state = 0;
			auto_state = 0;
			manual_state = 0;
			paused_state = 0;
			wash = 0;
			wash_time = 0;
			rinse = 0;
			rinse_time = 0;
			dry = 0;
			dry_time = 0;
			end_state = 0;
		}
	}
}

//실행하면 공통 인수로 넣고 비교해서 pause가 작동해야할 듯??
void init_timer0(void)
{
	TCNT0=6;   // TCNT 6~256 ==> 정확히 1ms 마다 TIMT 0 OVF INT
	// 분주비를 64로 설정 P269 표13-1
	TCCR0 |= (1 << CS02) | (0 << CS01) | (0 << CS00);

	TIMSK |= (1 << TOIE0);			// 오버플로 인터럽트 허용
	
}
