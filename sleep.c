#include "sleep.h"
<<<<<<< HEAD
#include <avr/power.h>
=======
>>>>>>> 5793541bd196650923ae10c25b167ddf9128cc26

void sleep_init()
{
	SMCR |= _BV(SM1) | _BV(SM0);	//select "power save" mode
}


<<<<<<< HEAD
extern void sleep_now(void)		//function for sleeping multiples of 1ms
{
	sei();				//enable interrupts if not aredy enabled
	power_twi_disable();
	power_usart0_disable(); 
	sleep_init();			//initialize sleep mode
	sleep_enable();			//enable sleep mode bit
	sleep_cpu();			//go to selected sleep mode
	
}
=======
extern void sleep_now(void)		//function to go to sleep
{
	sei();				//enable interrupts if not aredy enabled
	sleep_init();			//initialize sleep mode
	sleep_enable();			//enable sleep mode bit
	sleep_cpu();			//go to selected sleep mode
}

extern void timer2_1ms_setup()
{
	// set up timer 2 in normal mode with interrupt on overflow
	TCCR2A = 0x00; 

#if F_CPU == 1000000UL
	TCCR2B  |= _BV(CS21);	// prescaler of 8
#elif F_CPU == 8000000UL
	TCCR2B  |= _BV(CS22);	// prescaler of 64
#else
#error F_CPU not defined
#endif
	TIMSK2 	|= _BV(TOIE2); 	//enable timer 2 overflow interrupt
	//reset timer value to overflow in 125 counts (1ms)
	timer2_1ms_reset();
}

>>>>>>> 5793541bd196650923ae10c25b167ddf9128cc26
