#include "rtc.h"

volatile uint32_t RTC_counter;
volatile uint16_t ms_count;

extern void timer2_1ms_setup()
{
	// set up timer 2 in normal mode with interrupt on overflow
	TCCR2A = 0x00; 
	TCCR2B  |= _BV(CS22);	// prescaler of 64
	TIMSK2 	|= _BV(TOIE2); 	//enable timer 2 overflow interrupt

	//set timer value to 247
	TCNT2 = 130;
}

extern void RTC_get_dhms (uint32_t RTC_count, uint8_t * days, uint8_t * hours, uint8_t * minutes, uint8_t * seconds)
{
	days = 0;
	hours = 0;
	minutes = 0;
	seconds = 0;

	while( RTC_count-86400 >= 0)
	{
		RTC_count = RTC_count - 86400;
		days++;
	}
	while( RTC_count-3600 >=0)
	{
		RTC_count = RTC_count - 3600;
		hours++;
	}
	while( RTC_count-60 >=0)
	{
		RTC_count = RTC_count - 60;
		minutes++;
	}
		*seconds = RTC_count;

}

extern void RTC_set_dhms (uint32_t * RTC_count, uint8_t days, uint8_t hours, uint8_t minutes, uint8_t seconds)
{
	*RTC_count = (days*86400) + (hours*3600) + (minutes*60) + seconds;
}


