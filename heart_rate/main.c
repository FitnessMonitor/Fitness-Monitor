//RF transmission
#define F_CPU 8000000UL /* 8 MHz Internal Oscillator */
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
//#include "../spi.c"
//#include "../nRF24L01.c"
#include "../sleep.c"
#include "../ADC.c"
#include "../lcd.c"
#include "../rtc.c"
#include "../heart_rate.c"

//Macros for setting, clearing and toogleing bits.
#define SET_BIT(PORT, BITNUM) ((PORT) |= (1<<(BITNUM)))
#define CLEAR_BIT(PORT, BITNUM) ((PORT) &= ~(1<<(BITNUM)))
#define TOGGLE_BIT(PORT, BITNUM) ((PORT) ^= (1<<(BITNUM)))

volatile int8_t ADC_running;
volatile uint32_t RTC_sec_counter;
volatile uint16_t RTC_ms_counter;
volatile uint8_t hr_counterer;

ISR( PCINT2_vect ) 
{
	//nRF24L01_interrupt ();
}

ISR(TIMER2_OVF_vect)	//when timer 2 interrupts
{			//wake up from sleeping
	sleep_disable();
	timer2_1ms_reset();	//reset timer to interrupt in 1ms
	RTC_ms_counter++;
	
}

ISR(ADC_vect)
{
	//ADC_running = 0;
	//hr_samples[sample[0]] = ADCH;
}

int main(void){

	//initialize ports
	SET_BIT(DDRC,5);
	
	//initialize ADC
	ADC_init();
	
	//initialize LCD screen
	lcd_init(LCD_DISP_ON);
	lcd_clrscr();
	//lcd_puts("test");

	RTC_sec_counter = 0;
	
	int16_t days;
	int8_t hours;
	int8_t minutes;
	int8_t seconds;

	timer2_1ms_setup();


	char sec_str[2];
	char min_str[2];
	char hr_str[2];
	char day_str[3];
	while(1)
	{
		if(RTC_ms_counter >= 1000)
		{
			RTC_sec_counter++;
			RTC_get_dhms (&RTC_sec_counter, &days, &hours, &minutes, &seconds);
			lcd_puts("m");
			itoa(seconds, sec_str,10);
			itoa(minutes, min_str,10);
			itoa(hours, hr_str,10)
			lcd_clrscr();
			lcd_putc(hr_str[0]);
			lcd_putc(hr_str[1]);
			lcd_puts(":");
			lcd_putc(min_str[0]);
			lcd_putc(min_str[1]);
			lcd_puts(":");
			lcd_putc(sec_str[0]);
			lcd_putc(sec_str[1]);
			RTC_ms_counter = 0;
		}

		//go back to sleep now
		sleep_now();	// sleep for 1 ms
	}
}






















