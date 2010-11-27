#define F_CPU 1000000UL /* 1 MHz Internal Oscillator */
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "../lib/nRF24L01.c"
#include "../lib/spi.c"
#include "../lib/sleep.c"
#include "../lib/ADC.c"
#include "heart_rate.c"

volatile uint16_t ms_counter = 0;

ISR(TIMER2_OVF_vect)	//when timer 2 interrupts
{			//wake up from sleeping
	sleep_disable();
	timer2_1ms_reset();	//reset timer to interrupt in 1ms
	ms_counter++;
	//if(!(ms_counter % 10)) sleep_now();	//go back to sleep if not a 10ms interval
}

ISR(ADC_vect)
{
//hr_samples[sample[0]] = ADCH;
}

ISR( PCINT2_vect ) 
{
	nRF24L01_interrupt ();
}

int main(void)
{
	
	init_secondary_device();

	uint16_t hr_sample[150];
	uint8_t hr_index = 0;
	nRF24L01_data[0] = 0x00;
	uint16_t bpm = 0;

	while(1)
	{	
		if ((ms_counter == 50)	//sample the Heart Rate signal every 50ms
		{
			if (get_hr_sample() >= 80) SET_BIT(PORTD,2);	
			else CLEAR_BIT(PORTD,2);	
			ms_counter = 0;
		}
	
		}
		sleep_now();	// sleep until timer2 interrupt

	}
}

