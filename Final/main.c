#define F_CPU 1000000UL

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <string.h>
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
<<<<<<< HEAD
	PORTB = 0b00010101;
//	PORTB = 0b10110000; 	// Port B
=======
	PORTB = 0b10110000; 	// Port B
>>>>>>> parent of d3c2d09... PCB SD Card Problem
	DDRB  = 0b00000000;

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
}

int main (void)
{
	_delay_ms(1000);
	USARTInit(12);
	IoInit();
	setup();
	FATFS FileSystemObject;

	_delay_ms(100);

	DSTATUS driveStatus = disk_initialize(0);

	if(f_mount(0, &FileSystemObject)!=FR_OK) {
		// flag error
		_delay_ms(100);
	}

	drawstring(buffer, 0, 0, "Mount Filesystem");
	write_buffer(buffer);

	FIL logFile;

	if(f_open(&logFile, "/20101119.txt", FA_READ | FA_WRITE | FA_OPEN_ALWAYS)!=FR_OK) {
		//flag error
		_delay_ms(200);
	}
	else {
		drawstring(buffer, 0, 1, "Opened File");
		write_buffer(buffer);

		unsigned int bytesWritten;
		f_write(&logFile, "123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n", 100, &bytesWritten);

		f_write(&logFile, "987654321\n987654321\n987654321\n987654321\n987654321\n987654321\n987654321\n987654321\n987654321\n987654321\n", 100, &bytesWritten);
		//Flush the write buffer with f_sync(&logFile);

		drawstring(buffer, 0, 2, "Writing Done");
		write_buffer(buffer);

		//Close and unmount. 
		f_close(&logFile);
	}

	drawstring(buffer, 0, 3, "Program Complete");
	write_buffer(buffer);

}

