#define F_CPU 1000000UL

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <string.h>
#include <util/delay.h>
#include "ff.c"
#include "diskio.c"

#include "lcd.h"
#include "lcd.c"
#include "lcdfont.c"

#include "ADC.c"

uint8_t buffer[512];

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

void init_accelerometer(void) {
	ADC_init(ADC0);
	//ADC_init(ADC1);
	//ADC_init(ADC7);
}

uint8_t get_accelerometer_sample(void) {
	ADC_start_single_conversion();
	while((ADCSRA & (1<<ADSC))){};
	return ADCH;
}

int main (void)
{
	_delay_ms(500);
	setup();
	init_accelerometer();

	while(1) {
		drawstring( buffer, 0, 3, i2s(get_accelerometer_sample()) );
		write_buffer(buffer);
		_delay_ms(50);
	}
}

