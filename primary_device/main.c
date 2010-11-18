#define F_CPU 1000000UL
/*----------------------------------------------------------------------*/
/* FAT file system sample project for FatFs            (C)ChaN, 2010    */
/*----------------------------------------------------------------------*/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/power.h>
//#include <string.h>
#include <util/delay.h>
#include "ff.c"
#include "diskio.c"
#include "uart.c"

#include "lcd.h"
#include "lcd.c"
#include "lcdfont.c"

uint8_t buffer[512];

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

void setup(void) {
    // turn on backlight
    BLA_DDR |= _BV(BLA);
    BLA_PORT |= _BV(BLA);

    LED_DDR |= _BV(LED);

    st7565_init();
    st7565_command(CMD_DISPLAY_ON);
    st7565_command(CMD_SET_ALLPTS_NORMAL);
    clear_screen();
    clear_buffer(buffer);

    //testdrawchar(buffer);
    drawstring(buffer, 0, 0, "Dan Cole");
    drawstring(buffer, 0, 3, "Fitness Monitor");
    write_buffer(buffer);
    clear_buffer(buffer);
}

int main (void)
{
	char *ptr1 = "No SD Card \r\n";
	char *ptr2 = "Started \r\n ";
	char *ptr3 = "Working \r\n ";
	char dan;
	UINT s1;
	char BuffArr[100];
	char *Buff = &BuffArr[0];
	BYTE res;

	USARTInit(12);
	IoInit();
	setup();
	FATFS FileSystemObject;


	DSTATUS driveStatus = disk_initialize(0);
	
	if ((driveStatus & STA_NODISK) || (driveStatus & STA_NOINIT)){
   		drawstring(buffer, 0, 0, "ERROR");
   		write_buffer(buffer);	
	}
	else{
		if(f_mount(0, &FileSystemObject)!=FR_OK) {
			//PORTC |= (1<<PC2);
	    		drawstring(buffer, 0, 0, "ERROR");
	   		write_buffer(buffer);
		}
		else{
			drawstring(buffer, 0, 1, "Mounted");
	   		write_buffer(buffer);
		}


		FIL logFile;



		if(f_open(&logFile, "/20101117.txt", FA_READ | FA_WRITE | FA_OPEN_ALWAYS)!=FR_OK) {
			drawstring(buffer, 0, 2, "File not created");
	   		write_buffer(buffer);
		}
		else{
			drawstring(buffer, 0, 2, "File created");
	   		write_buffer(buffer);
			unsigned int bytesWritten;
			f_write(&logFile, "123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n", 100, &bytesWritten);
			f_write(&logFile, "987654321\n987654321\n987654321\n987654321\n987654321\n987654321\n987654321\n987654321\n987654321\n987654321\n", 100, &bytesWritten);
			drawstring(buffer, 0, 3, "File Written");
	   		write_buffer(buffer);
			//Close and unmount. 
			f_close(&logFile);
		}
	}
	// sizeof(Buff)
/*
	if(f_read (&logFile, Buff, 100, &s1) != FR_OK) {
		while(1) {
			PORTC ^= (1<<PC3);
			_delay_ms(100);
		}
	}
	uart_puts(Buff);
	_delay_ms(1000);
*/
/*
	while(1) {
		PORTC ^= (1<<PC3);
		_delay_ms(100);
	}
*/
	//works


	

	//Flush the write buffer with f_sync(&logFile);





/*
	if (driveStatus & STA_NOINIT) {
		PORTC |= (1<<PC3);
	}
	else {
		while(1) {
			PORTC ^= (1<<PC3);
			_delay_ms(100);
		}
	}
*/
    	drawstring(buffer, 0, 4, "Done");
   	write_buffer(buffer);

	while(1) {	
		uart_puts(ptr3);
		_delay_ms(1000);	
	}
/*
	//_delay_ms(1000);

	DSTATUS driveStatus = disk_initialize(0);

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

