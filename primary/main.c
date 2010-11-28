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
	uint8_t seconds = 1;
	uint8_t minutes = 0;
	uint8_t hours = 0;

	//used for calculating steps & activity
	uint8_t accel_index = 0;
	uint8_t xaxis [100];
	uint8_t xavg;
	uint8_t steps;
	uint16_t step_count;
	uint8_t activity_level;
	uint16_t activity_sum;


	//arrays to store data between writes
	uint8_t heart_rate[15];
	uint16_t steps_delta[15];
	uint8_t	activity[15];
	uint8_t store_index = 0;

	//used for writing data
	char data_string[100];
	unsigned int bytesWritten;

	///initialize
	setup();
	init_sdcard();
	//sdcard_open(hours, minutes, seconds);

	// initialize timer 2 to interrupt ever 1ms
	timer2_1ms_setup();
	while(1)
	{	
		if (ms_counter % 50)	// sample every 50ms		
		{	
			//sample the Acceleromiter 
			xaxis[accel_index++] = get_adc_sample(0);

			// Dan Cole's test area
			/*
			unsigned int bytesWritten;
			uint8_t sample;
			char sample_text[4];
			sample = get_adc_sample(0);
			xaxis[accel_index++] = sample;
			char *sdcard_text = &sample_text[0];
			sprintf(sdcard_text, "%d    ", (int) sample );
			f_write(&logFile, sdcard_text, 4, &bytesWritten);
			f_write(&logFile, "\n", 1, &bytesWritten);
			*/
		}
		if (ms_counter >= 1000) // every 1 seconds
		{	
			sdcard_close();
			drawstring(disp_buffer, 0, 4, "SD Card Closed");
			write_buffer(disp_buffer);

			ms_counter = 0; // reset counter
			seconds ++;
			accel_index = 0;
			clear_buffer(&disp_buffer[0]);
			disp_hms(hours, minutes, seconds);
			get_steps(&xaxis[0], 20, &xavg, &steps, &activity_level);
			//step_count += steps;
			//activity_sum+=activity_level;

			if (seconds >= 60) // every 1 minute
			{
				minutes ++;
				seconds = 0;
				if (minutes >= 60)
				{
					hours++;
					minutes = 0;
				} //endif

				//store the number of steps since the last minute
				steps_delta[store_index] = 7;//step_count;
				step_count = 0;

				//store the value recieved 
				heart_rate[store_index] = 56;//nRF24L01_data[0]; 
				nRF24L01_data[0] = 0;
				//nRF24L01_RX_powerup(); //turn on reciever to recive next heart_rate package;
				
				//store the activity level;
				
				activity[store_index] = 10;///activity_sum / 10;
				activity_sum = 0;
				store_index++; //increment the index


 				if (minutes == 1) // every 10 minutes
				{	
					store_index = 0;

					//Write the data to the SD card 
					sdcard_open(hours, minutes, seconds);
					int k;
					for (k = 0; k < 10; k++)
					{			
					//each minute gets its own line		
					sprintf( &data_string[0], "%d,%d,%d,      ", (int) steps_delta[k], (int) heart_rate[k], (int) activity[k] );
					f_write(&logFile, &data_string[0], 10, &bytesWritten);
					f_write(&logFile, "\n", 1, & bytesWritten);
					}
					sdcard_close();
				} //endif
			} //endif	
		} //endif
		sleep_now();	// sleep until timer2 interrupt
	}//endwhile
}

