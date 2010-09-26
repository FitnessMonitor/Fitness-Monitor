#define F_CPU 8000000UL
/*----------------------------------------------------------------------*/
/* FAT file system sample project for FatFs            (C)ChaN, 2010    */
/*----------------------------------------------------------------------*/


#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>
#include "ff.h"
#include "ff.c"
#include "mmc.c"
#include "xitoa.h"
#include "diskio.h"
#include "ccsbcs.c"
#include "uart.h"
#include "uart.c"
#include "rtc.c"


DWORD acc_size;				/* Work register for fs command */
WORD acc_files, acc_dirs;
FILINFO Finfo;
#if _USE_LFN
char Lfname[_MAX_LFN+1];
#endif


char Line[128];				/* Console input buffer */

FATFS Fatfs[_VOLUMES];		/* File system object for each logical drive */
BYTE Buff[1024];			/* Working buffer */

volatile WORD Timer;		/* 100Hz increment timer */



#if _MULTI_PARTITION != 0
const PARTITION Drives[] = { {0,0}, {0,1} };
#endif

/*---------------------------------------------------------*/
/* 100Hz timer interrupt generated by OC2                  */
/*---------------------------------------------------------*/


ISR(TIMER2_COMPA_vect)
{
	Timer++;			/* Performance counter for this module */
	disk_timerproc();	/* Drive timer procedure of low level disk I/O module */
}



/*---------------------------------------------------------*/
/* User Provided Timer Function for FatFs module           */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support a real time clock.          */
/* This is not required in read-only configuration.        */


DWORD get_fattime ()
{
	RTC rtc;


	/* Get local time */
	rtc_gettime(&rtc);

	/* Pack date and time into a DWORD variable */
	return	  ((DWORD)(rtc.year - 1980) << 25)
			| ((DWORD)rtc.month << 21)
			| ((DWORD)rtc.mday << 16)
			| ((DWORD)rtc.hour << 11)
			| ((DWORD)rtc.min << 5)
			| ((DWORD)rtc.sec >> 1);
}


/*--------------------------------------------------------------------------*/
/* Monitor                                                                  */


static
void put_dump (const BYTE *buff, DWORD ofs, BYTE cnt)
{
	BYTE i;


	xprintf(PSTR("%08lX "), ofs);

	for(i = 0; i < cnt; i++)
		xprintf(PSTR(" %02X"), buff[i]);

	xputc(' ');
	for(i = 0; i < cnt; i++)
		xputc((buff[i] >= ' ' && buff[i] <= '~') ? buff[i] : '.');

	xputc('\n');
}


static
void get_line (char *buff, int len)
{
	BYTE c;
	int i = 0;


	for (;;) {
		c = uart_get();
		if (c == '\r') break;
		if ((c == '\b') && i) {
			i--;
			uart_put(c);
			continue;
		}
		if (c >= ' ' && i < len - 1) {	/* Visible chars */
			buff[i++] = c;
			xputc(c);
		}
	}
	buff[i] = 0;
	uart_put('\n');
}


static
FRESULT scan_files (
	char* path		/* Pointer to the working buffer with start path */
)
{
	DIR dirs;
	FRESULT res;
	int i;
	char *fn;

	res = f_opendir(&dirs, path);
	if (res == FR_OK) {
		i = strlen(path);
		while (((res = f_readdir(&dirs, &Finfo)) == FR_OK) && Finfo.fname[0]) {
			if (_FS_RPATH && Finfo.fname[0] == '.') continue;
#if _USE_LFN
			fn = *Finfo.lfname ? Finfo.lfname : Finfo.fname;
#else
			fn = Finfo.fname;
#endif
			if (Finfo.fattrib & AM_DIR) {
				acc_dirs++;
				*(path+i) = '/'; strcpy(path+i+1, fn);
				res = scan_files(path);
				*(path+i) = '\0';
				if (res != FR_OK) break;
			} else {
//				xprintf(PSTR("%s/%s\n"), path, fn);
				acc_files++;
				acc_size += Finfo.fsize;
			}
		}
	}

	return res;
}



static
void put_rc (FRESULT rc)
{
	const prog_char *p;
	static const prog_char str[] =
		"OK\0" "DISK_ERR\0" "INT_ERR\0" "NOT_READY\0" "NO_FILE\0" "NO_PATH\0"
		"INVALID_NAME\0" "DENIED\0" "EXIST\0" "INVALID_OBJECT\0" "WRITE_PROTECTED\0"
		"INVALID_DRIVE\0" "NOT_ENABLED\0" "NO_FILE_SYSTEM\0" "MKFS_ABORTED\0" "TIMEOUT\0"
		"LOCKED\0" "NOT_ENOUGH_CORE\0" "TOO_MANY_OPEN_FILES\0";
	FRESULT i;

	for (p = str, i = 0; i != rc && pgm_read_byte_near(p); i++) {
		while(pgm_read_byte_near(p++));
	}
	xprintf(PSTR("rc=%u FR_%S\n"), rc, p);
}


static void IoInit ()
{
	PORTB = 0b10110000; 	// Port B
	DDRB  = 0b00001000;

	DDRC |= 1<<PC3;
	DDRC |= 1<<PC2;

/*
	TIMSK0 |= 1 << OCIE0A;  //enable interrupt for timer match a 
	OCR0A = 78;  // 10 ms interrupt at 8MHz
	TCCR0B |= (1 << CS02) | (1 << CS00);  //speed = F_CPU/1024
	//power_timer0_enable();

	sei();
*/

	//uart_init();		// Initialize UART driver

/*
	OCR1A = 51;			// Timer1: LCD bias generator (OC1B)
	OCR1B = 51;
	TCCR1A = 0b00010000;
	TCCR1B = 0b00001010;
*/
	//OCR2A = 90-1;		// Timer2: 100Hz interval (OC2)
	//TCCR2A = 0b00000010;
	//TCCR2B = 0b00000101;
	//TIMSK2 = 0b00000010;   // Enable TC2.oc interrupt 

	//rtc_init();			// Initialize RTC

//	sei();
}

void delayms(uint16_t millis) {
  while ( millis ) {
    _delay_ms(1);
    millis--;
  }
}

int main (void)
{

	IoInit();
	FATFS FileSystemObject;

	if(f_mount(0, &FileSystemObject)!=FR_OK) {
		//flag error.
	}

	delayms(5000);

	DSTATUS driveStatus = disk_initialize(0);

// driveStatus & STA_NOINIT || driveStatus & STA_NODISK || driveStatus & STA_PROTECT
// With No SD Card	driveStatus = 1,	STA_NOINIT = 1, 	STA_NODISK = 1, 	STA_PROTECT = 1
// With SD Card		driveSratus = 1,
	if(driveStatus & STA_NODISK) {
		while(1) 
		{
			PORTC ^= (1<<PC3);
			PORTC ^= (1<<PC2);
			delayms(200);
		}
	}
	else {
		while(1) 
		{
			PORTC ^= (1<<PC3);
			PORTC ^= (1<<PC2);
			delayms(1000);
		}
	}

/*
	DDRC |= 1<<PC2;
	FATFS FileSystemObject;

	if(f_mount(0, &FileSystemObject)!=FR_OK) {
		PORTC |= (1<<PC2);
	}

	DSTATUS driveStatus = disk_initialize(0);

	if(driveStatus & STA_NOINIT || driveStatus & STA_NODISK || driveStatus & STA_PROTECT) {
		//flag error.
	}

	//  Sometimes you may want to format the disk.
	if(f_mkfs(0,0,0)!=FR_OK) {
		//error
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
*/
}

