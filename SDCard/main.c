#define F_CPU 8000000UL
/*----------------------------------------------------------------------*/
/* FAT file system sample project for FatFs            (C)ChaN, 2010    */
/*----------------------------------------------------------------------*/


#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <string.h>
#include <util/delay.h>
#include "ff.c"
#include "diskio.c"

ISR(TIMER0_COMPA_vect) {  /* should be called every 10ms */
  disk_timerproc();
}

static void IoInit ()
{
	PORTB = 0b10110000; 	// Port B
	DDRB  = 0b00001000;

	DDRC |= 1<<PC3;
	DDRC |= 1<<PC2;

	TIMSK0 |= 1 << OCIE0A;  //enable interrupt for timer match a 
	OCR0A = 78;  // 10 ms interrupt at 8MHz
	TCCR0B |= (1 << CS02) | (1 << CS00);  //speed = F_CPU/1024
	power_timer0_enable();

	sei();
}

int main (void)
{
	
	IoInit();
	DESELECT();
	FATFS FileSystemObject;
/*
	if(f_mount(0, &FileSystemObject)!=FR_OK) {
		PORTC |= (1<<PC2);
	}

	if (Stat & STA_NODISK) {
		PORTC |= (1<<PC3);
	}
	else {
		while(1) {
			PORTC ^= (1<<PC3);
		}
	}

	//_delay_ms(1000);
*/
	DSTATUS driveStatus = disk_initialize(0);

	if(driveStatus & STA_NOINIT || driveStatus & STA_NODISK || driveStatus & STA_PROTECT) {
		//flag error.
	}


	FIL logFile;
	//works
	if(f_open(&logFile, "/dancole.txt", FA_READ | FA_WRITE | FA_OPEN_ALWAYS)!=FR_OK) {
		//flag error
	}

	unsigned int bytesWritten;
	f_write(&logFile, "New log opened!\n", 16, &bytesWritten);
	//Flush the write buffer with f_sync(&logFile);

	//Close and unmount. 
	f_close(&logFile);
	f_mount(0,0);

}

