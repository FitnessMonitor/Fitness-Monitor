#include "sleep.h"
#include "rtc.c"

void sleep_init()
{
	SMCR |= _BV(SM1) | _BV(SM0);	//select "power save" mode
}


extern void sleep_now(void)		//function for sleeping multiples of 1ms
{
	sei();				//enable interrupts if not aredy enabled
	sleep_init();			//initialize sleep mode
	sleep_enable();			//enable sleep mode bit
	sleep_cpu();			//go to selected sleep mode
	
}
