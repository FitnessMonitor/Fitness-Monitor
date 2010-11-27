#define F_CPU 1000000UL /* 1 MHz Internal Oscillator */
#include <avr/io.h>
#include <util/delay.h>
//#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <stdlib.h>
#include <stdio.h>
#include "../lib/nRF24L01.c"
#include "../lib/spi.c"
#include "../lib/sleep.c"
#include "../lib/ADC.c"
#include "../lib/ff.c"
#include "../lib/diskio.c"
#include "../lib/lcd.c"
#include "../lib/lcdfont.c"

volatile uint16_t ms_counter = 0;
uint8_t disp_buffer[512];
FATFS FileSystemObject;
FIL logFile;

#include "primary.c"

ISR(TIMER2_OVF_vect)	//when timer 2 interrupts
{			//wake up from sleeping
	sleep_disable();
	timer2_1ms_reset();	//reset timer to interrupt in 1ms
	ms_counter++;
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
	//variables for keeping track of time
	uint8_t seconds = 0;
	uint8_t minutes = 0;
	uint8_t hours = 0;

	//used for calculating steps
	uint8_t accel_index = 0;
	uint8_t xaxis [100];


	//arrays to store data between writes
	uint8_t 	heart_rate[15];
	uint16_t	steps_delta[15];
	uint8_t		activity[15];
	uint8_t		store_index;

	//initialize
	setup();
	init_sdcard();
	sdcard_open(&minutes);
	// initialize timer 2 to interrupt ever 1ms
	timer2_1ms_setup();
	while(1)
	{	
		if (ms_counter % 50)	// sample every 50ms		
		{	
			//sample the Acceleromiter
			xaxis[accel_index++] = get_adc_sample(0);
		}
		if (ms_counter >= 1000) // every 1 seconds
		{	
			ms_counter = 0; // reset counter
			seconds ++;
			accel_index = 0;

			// do something to process the acceleromiter samples

			disp_hms(hours, minutes, seconds);

			if (seconds >= 60) // every 1 minute
			{
				//updtae counters
				seconds = 0;	//reset seconds
				minutes ++;	//increment minutes

				//store the number of steps since the last minute
				steps_delta[store_index] = 0;

				//store the value recieved 
				heart_rate[store_index] = nRF24L01_data[0]; 
				//nRF24L01_RX_powerup(); //turn on reciever to recive next heart_rate package;

				store_index++; //increment the index

 				if (minutes % 10) // every 10 minutes
				{	
					//Write the data to the SD card
					if (minutes == 60)
					{
						hours++;
						minutes = 0;
					} //endif
				} //endif
			} //endif	
		} //endif
		sleep_now();	// sleep until timer2 interrupt
	}//endwhile
}











