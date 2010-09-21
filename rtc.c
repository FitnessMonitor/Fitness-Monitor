#include "rtc.h"

extern void timer2_1ms_setup()
{
	// set up timer 2 in normal mode with interrupt on overflow
	TCCR2A = 0x00; 
	TCCR2B  |= _BV(CS22);	// prescaler of 64
	TIMSK2 	|= _BV(TOIE2); 	//enable timer 2 overflow interrupt

	timer2_1ms_reset();
}

extern void RTC_get_dhms (volatile uint32_t * RTC_count, int16_t * days, int8_t * hours, int8_t * minutes, int8_t * seconds)
{
	int32_t d = 0;
	int32_t h = 0;
	int32_t m = 0;
	int32_t s = 0;
	uint32_t count;
	count = *RTC_count;
		
	d = count/86400;
	count = count - (d * 86400);
	h = count/3600;
	count = count - (h * 3600);
	m = count/60;
	count = count - (m * 60);
	s = count;

	*days = d;
	*hours = h;
	*minutes = m;
	*seconds = s;

}
//4294967296

extern void RTC_set_dhms (volatile uint32_t * RTC_count, int16_t days, int8_t hours, int8_t minutes, int8_t seconds)
{
	*RTC_count = (days * 86400) + (hours * 3600) + (minutes * 60) + seconds;
}



