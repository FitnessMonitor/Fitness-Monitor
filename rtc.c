#include "rtc.h"

volatile int8_t RTC_hours;
volatile int8_t RTC_minutes;
volatile int8_t RTC_seconds;
volatile int8_t RTC_milisec;

extern void timer2_1ms_setup()
{
	// set up timer 2 in normal mode with interrupt on overflow
	TCCR2A = 0x00; 
	TCCR2B  |= _BV(CS22) | _BV(CS21) | _BV(CS20);	// prescaler of 1024
	TIMSK2 	|= _BV(TOIE2); 	//enable timer 2 overflow interrupt

	//set timer value to 247
	TCNT2 = 247;
}
/*
extern void RTC_get_dhms (.......)
{
	keep time value in seconds
}
extern void RTC_set_dhms (.......)
{
}
*/
extern void update_time()
{
	//this should change to something that stores the number of seconds 
	//from a given point
	RTC_milisec++;
	if (RTC_milisec > 1000)
	{
		RTC_milisec = 0;
		RTC_seconds++;
		if (RTC_seconds > 60);
		{
			RTC_seconds = 0;
			RTC_hours++;
			if (RTC_hours > 24)
			{
				RTC_hours = 0;
			}
		}	
	}
}

