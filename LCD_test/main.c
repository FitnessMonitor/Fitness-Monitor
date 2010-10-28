#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "glcd.h"
#include "glcd.c"
#include "glcdfont.c"


//uint8_t is_reversed = 0;

//int pagemap[] = { 3, 2, 1, 0, 7, 6, 5, 4 };

volatile uint8_t buffer[512] ;

int main(void) {
  setup();
  while (1) {
    loop();
  }
}

void loop(void) {}

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

   // testdrawchar(buffer);
    drawstring(buffer, 0, 0, "Dan Charles Norm test test test");
    drawstring(buffer, 0, 3, "Dan Charles Norm");
    write_buffer(buffer);
}




