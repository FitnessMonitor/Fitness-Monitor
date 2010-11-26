#define F_CPU 1000000UL /* 8 MHz Internal Oscillator */
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

volatile uint32_t ms_counter = 0;
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

void init_sdcard(void)
{
	DSTATUS driveStatus = disk_initialize(0);

	if(f_mount(0, &FileSystemObject)!=FR_OK) {
		//flag error
		drawstring( disp_buffer, 0, 1, "File System Mounting Error" );
		write_buffer(disp_buffer);
	}
}

void sdcard_open(uint8_t *name)
{
	char file_name[16];
	sprintf( file_name, "/%n.txt", (int *) name );
	if(f_open(&logFile, file_name, FA_READ | FA_WRITE | FA_OPEN_ALWAYS)!=FR_OK) {
		//flag error
		drawstring( disp_buffer, 0, 1, "f_open Error" );
		write_buffer(disp_buffer);
	}
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

	setup();
	//init_sdcard();
	//sdcard_open(minutes);
	// initialize timer 2 to interrupt ever 1ms
	timer2_1ms_setup();
	while(1)
	{	
		if (ms_counter % 50)	// sample every 50ms		
		{
			xaxis[accel_index++] = get_adc_sample(0);
		}
		if (ms_counter % 1000) // every 5 seconds
		{
			accel_index = 0;
			unsigned int bytesWritten;
			//f_write(&logFile, xaxis, 100, &bytesWritten);
			seconds += 1;
			//sprintf( display_seconds, "%d seconds", 1 );
			i2s((int)seconds, display_seconds);
			drawstring( disp_buffer, 0, 0, display_seconds );
			write_buffer(disp_buffer);
		}
		if (ms_counter % 60000) // every 1 minute
		{
			drawstring( disp_buffer, 0, 1, "1 minute" );
			write_buffer(disp_buffer);
			//sdcard_close();
			//sdcard_open(&minutes);
		}
 		if (ms_counter == 600000) // every 10 minutes
		{
			ms_counter = 0; // reset counter
			if (minutes == 60)
			{
				hours++;
				minutes = 0;
			}
			drawstring( disp_buffer, 0, 2, "10 minutes" );
			write_buffer(disp_buffer);
		}

		sleep_now();	// sleep until timer2 interrupt
	}
	f_mount(0,0);
}

