#define F_CPU 1000000UL
/*----------------------------------------------------------------------*/
/* FAT file system sample project for FatFs            (C)ChaN, 2010    */
/*----------------------------------------------------------------------*/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <string.h>
#include <util/delay.h>

int main (void)
{
	DDRB |= 1<<PC3; /* set to output */
	DDRB |= 1<<PC4; /* set to output */
	DDRB |= 1<<PC5; /* set to output */
	while(1) {	
		PORTC ^= 0x38; /* LEDs*/
		_delay_ms(1000);	
	}
}

