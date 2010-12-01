#define F_CPU 1000000UL /* 1 MHz Internal Oscillator */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
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
	uint16_t xavg = 0;
	uint16_t steps = 0;
	uint16_t step_count = 0;
	uint16_t activity_level = 0;
	uint16_t activity_sum = 0;


	//arrays to store data between writes
	uint16_t heart_rate[15];
	uint16_t steps_delta[15];
	uint16_t activity[15];
	uint8_t store_index = 0;

	//used for writing data
	char data_string[100];
	unsigned int bytesWritten;


	// Initialize AVR for use with nRF24L01
	//nRF24L01_init();
	// Wait for nRF24L01 to come up

	// Configure nRF24L01
	//nRF24L01_config();
	//nRF24L01_powerdown();

	///initialize
	setup();
	init_sdcard();
	//sdcard_open(hours, minutes, seconds);

	// initialize timer 2 to interrupt ever 1ms
	timer2_1ms_setup();

	while(1)
	{	
		if ((ms_counter % 50) == 0)	// sample every 50ms		
		{	
			//sample the Acceleromiter 
			xaxis[accel_index++] = get_adc_sample(0);
		}
		if (ms_counter >= 1000) // every 1 seconds
		{	
			drawstring(disp_buffer, 0, 4, "SD Card Closed");
			write_buffer(disp_buffer);

			ms_counter = 0; // reset counter
			seconds ++;
			accel_index = 0;
			//clear_buffer(&disp_buffer[0]);
			disp_hms(hours, minutes, seconds);
			get_steps(&xaxis[0], 20, &xavg, &steps, &activity_level);
			step_count += steps;
			activity_sum += activity_level;


			char display_str[40];
			sprintf( &display_str[0], "steps:%d activity:%d", (int) step_count, (int) activity_level );
			drawstring( disp_buffer, 0, 2, &display_str[0] );
			write_buffer(disp_buffer);


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
				steps_delta[store_index] = step_count;
				step_count = 0;

				//store the value recieved 
				heart_rate[store_index] = nRF24L01_data[0]; 
				nRF24L01_data[0] = 0;
				//nRF24L01_RX_powerup(); //turn on reciever to recive next heart_rate package;
				
				//store the activity level;
				
				activity[store_index] = (activity_sum / 60);
				activity_sum = 0;
				store_index++; //increment the index

 				if ((minutes % 10) == 0) // every 10 minutes
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

