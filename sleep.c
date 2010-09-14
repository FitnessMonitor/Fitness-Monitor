#include "sleep.h"

void sleep_init()
{
	SMCR |= _BV(SM1) | _BV(SM0);	//select "power save" mode
	
	// set up timer 2 in normal mode with interrupt on overflow
	TCCR2A = 0x00; 
	TCCR2B  |= _BV(CS22) | _BV(CS21) | _BV(CS20);	// prescaler of 1024
	TIMSK2 	|= _BV(TOIE2); 	//enable timer 2 overflow interrupt

	//set timer value to 247
	TCNT2 = 247;
}

void sleep_timer_destroy()
{
	//disable timer 2 interrupts
	TIMSK2 = 0;
	sleep_disable();	//return from sleeping
}

extern void sleep_ms ( uint16_t loops)		//function for sleeping multiples of 1ms
{
	sei();
	int8_t sleep_count;
	for (sleep_count = 0; sleep_count < loops; sleep_count++)
	{
		sleep_init();
		TCNT2 = 247;			//set timer value to 247
		sleep_enable();			//enable sleep mode bit
		sleep_cpu();			//go to selected sleep mode
	}
	sleep_timer_destroy();	
}
