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


static void IoInit ()
{
	PORTB = 0b10110000; 	// Port B
	DDRB  = 0b00001000;

	DDRC |= 1<<PC3;
	DDRC |= 1<<PC2;

	sei();

}


void setup(void) {
	// turn on backlight
	BLA_DDR |= _BV(BLA);
	BLA_PORT |= _BV(BLA);

	LED_DDR |= _BV(LED);
	IoInit();
	
	st7565_init();
	st7565_command(CMD_DISPLAY_ON);
	st7565_command(CMD_SET_ALLPTS_NORMAL);
	clear_screen();
	clear_buffer(disp_buffer);

	//testdrawchar(disp_buffer);
	drawstring(disp_buffer, 0, 3, "Fitness Monitor");
	write_buffer(disp_buffer);
	clear_buffer(disp_buffer);
}

int init_sdcard(void)
{
	DSTATUS driveStatus = disk_initialize(0);
	if ((driveStatus & STA_NODISK) || (driveStatus & STA_NOINIT)) //check for initialization errors
	{
   		drawstring(disp_buffer, 0, 1, "SD Initialization Error");
   		write_buffer(disp_buffer);
		return 1;	//error 1 initialization failed
	}
	else{	//dont try mounting if initialization failed
		if(f_mount(0, &FileSystemObject)!=FR_OK) {
			//flag error
			drawstring( disp_buffer, 0, 1, "File System Mounting Error" );
			write_buffer(disp_buffer);
			return 2;	//error 2 mounting failed
		}
	}
	return 0; //no errors
}

int sdcard_open(uint8_t *name)
{
	char file_name[16];
	sprintf( file_name, "/%n.txt", (int *) name );
	if(f_open(&logFile, "20101127.txt", FA_READ | FA_WRITE | FA_OPEN_ALWAYS)!=FR_OK) {
		//flag error
		drawstring( disp_buffer, 0, 1, "f_open Error" );
		write_buffer(disp_buffer);
		return 1;
	}
	return 0;
}
void sdcard_close()
{
	f_close(&logFile);
}

int main(void)
{
	uint8_t seconds = 0;
	uint8_t minutes = 0;
	uint8_t hours = 0;
	uint8_t accel_index = 0;
	uint8_t xaxis [100];
	char display_seconds[10];

<<<<<<< HEAD
=======
	//used for writing data
	char file_name[10];
	char data_string[100];
	unsigned int bytesWritten;

	///initialize
>>>>>>> 03c176fc7b374dac6ae82cdd4f61202e8696bb64
	setup();
	init_sdcard();
	sdcard_open(&minutes);
	// initialize timer 2 to interrupt ever 1ms
	timer2_1ms_setup();
	while(1)
	{	
		if (ms_counter % 50)	// sample every 50ms		
<<<<<<< HEAD
		{
=======
		{	
			//sample the Acceleromiter 
			xaxis[accel_index++] = get_adc_sample(0);

			// Dan Cole's test area
>>>>>>> 03c176fc7b374dac6ae82cdd4f61202e8696bb64
			unsigned int bytesWritten;
			uint8_t sample;
			char sample_text[4];
			sample = get_adc_sample(0);
			xaxis[accel_index++] = sample;
			char *sdcard_text = &sample_text[0];
<<<<<<< HEAD
			sprintf(sdcard_text, "%d     ", (int) sample );
			f_write(&logFile, sdcard_text, 6, &bytesWritten);
			f_write(&logFile, "\n", 1, &bytesWritten);
		}
		if (ms_counter >= 1000) // every 1 seconds
		{
=======
			sprintf(sdcard_text, "%d\n", (int) sample );
			f_write(&logFile, sdcard_text, 4, &bytesWritten);
			
		}
		if (ms_counter >= 1000) // every 1 seconds
		{	
			sdcard_close();
>>>>>>> 03c176fc7b374dac6ae82cdd4f61202e8696bb64
			ms_counter = 0; // reset counter
			accel_index = 0;
			//unsigned int bytesWritten;
			//f_write(&logFile, "This is a test of time.\n", 24, &bytesWritten);
			sprintf( display_seconds, "%d seconds", (int) seconds );
			drawstring( disp_buffer, 0, 0, display_seconds );
			write_buffer(disp_buffer);
			seconds += 1;

			if (seconds == 60) // every 1 minute
			{
				sdcard_close();
				drawstring( disp_buffer, 0, 1, "1 minute" );
				write_buffer(disp_buffer);
				//sdcard_open(&minutes);
			}
		
<<<<<<< HEAD
 			if (minutes % 10) // every 10 minutes
=======
			//get_steps(&xaxis[0], 20, &xavg, &steps, &activity_level);
			//step_count += steps;
			//activity_sum+=activity_level;

			if (seconds >= 60) // every 1 minute
>>>>>>> 03c176fc7b374dac6ae82cdd4f61202e8696bb64
			{
				if (minutes == 60)
				{
					hours++;
					minutes = 0;
				}
				drawstring( disp_buffer, 0, 2, "10 minutes" );
				write_buffer(disp_buffer);
			}
		}

		sleep_now();	// sleep until timer2 interrupt
	}
	f_mount(0,0);
}

