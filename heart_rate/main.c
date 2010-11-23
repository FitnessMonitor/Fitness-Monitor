#define F_CPU 1000000UL /* 1 MHz Internal Oscillator */
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "../nRF24L01.c"
#include "../spi.c"
#include "../sleep.c"
#include "../ADC.c"
#include "../heart_rate.c"

//Macros for setting, clearing and toogleing bits.
#define SET_BIT(PORT, BITNUM) ((PORT) |= (1<<(BITNUM)))
#define CLEAR_BIT(PORT, BITNUM) ((PORT) &= ~(1<<(BITNUM)))
#define TOGGLE_BIT(PORT, BITNUM) ((PORT) ^= (1<<(BITNUM)))

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

int main(void){
	
	init_secondary_device();

	uint16_t hr_sample[150];
	uint8_t hr_index = 0;
	nRF24L01_data[0] = 0x00;
	uint16_t bpm = 0;

	while(1)
	{	
		if ((ms_counter % 50) && (ms_counter >=1000) && (ms_counter <=9000))	//sample the Heart Rate signal every 50ms
		{
			hr_sample[hr_index] = get_hr_sample();		
			hr_index++; //increment index of array
		}

		if (ms_counter > 9000)	//calculate bpm for this 10sec interval
		{
			bpm = calculate_bpm(&hr_sample[0], hr_index-1);					
		}		
		
		if (ms_counter > 10000)		//on every 10000ms (10sec)
		{
			//reset counter
			ms_counter = 0;

			//Transmit 
			nRF24L01_data[0] = bpm;
			nRF24L01_send(buffer,1);	
		}
		sleep_now();	// sleep until timer2 interrupt
	}
}






























