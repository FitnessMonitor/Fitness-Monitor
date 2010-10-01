#include "sleep.h"
#include <avr/power.h>

void sleep_init()
{
	SMCR |= _BV(SM1) | _BV(SM0);	//select "power save" mode
}


extern void sleep_now(void)		//function for sleeping multiples of 1ms
{
	sei();				//enable interrupts if not aredy enabled
	power_twi_disable();
	power_usart0_disable(); 
	sleep_init();			//initialize sleep mode
	sleep_enable();			//enable sleep mode bit
	sleep_cpu();			//go to selected sleep mode
	
}
