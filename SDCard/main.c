#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/power.h>

#include "discio.c"
#include "ff.c"

//Macros for setting, clearing and toggleing bits.
#define SET_BIT(PORT, BITNUM) ((PORT) |= (1<<(BITNUM)))    //or
#define CLEAR_BIT(PORT, BITNUM) ((PORT) &= ~(1<<(BITNUM))) //and
#define TOGGLE_BIT(PORT, BITNUM) ((PORT) ^= (1<<(BITNUM))) //exclusive or

ISR(TIMER0_COMPA_vect) {  /* should be called every 10ms */
	disk_timerproc();
}
	 
int main() {
	TIMSK0 |= 1 << OCIE0A;  /* enable interrupt for timer match a */
	OCR0A = 78;  /* 10 ms interrupt at 8MHz */
	TCCR0B |= (1 << CS02) | (1 << CS00);  //speed = F_CPU/1024
	power_timer0_enable();
 
	sei();				//Enable interrupts

	FATFS FileSystemObject;
	 
	if(f_mount(0, &FileSystemObject)!=FR_OK) {
	  //flag error
 	}
 
	DSTATUS driveStatus = disk_initialize(0);
 
	if(driveStatus & STA_NOINIT || driveStatus & STA_NODISK || driveStatus & STA_PROTECT) 
	{
  		//flag error.
	}
 
  	/*  Sometimes you may want to format the disk.
	if(f_mkfs(0,0,0)!=FR_OK) {
	    //error
	}
	*/
 
	FIL logFile;
	//works
	if(f_open(&logFile, "/GpsLog.txt", FA_READ | FA_WRITE | FA_OPEN_ALWAYS)!=FR_OK) {
	  	//flag error
	}

	unsigned int bytesWritten;
	f_write(&logFile, "New log opened!\n", 16, &bytesWritten);
	//Flush the write buffer with f_sync(&logFile);
	 
	//Close and unmount.
	f_close(&logFile);
	f_mount(0,0);
	return 0;
}
