#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "lcd.c"
#include "lcdfont.c"

// For (FTDI) USB UART 
#include <string.h>
#include <util/delay.h>
#include "uart.c"


volatile uint8_t buffer[512] ;

int main(void) {
  char *ptr1 = "Start \r\n";
  char *ptr2 = "Setup \r\n";
  char *ptr3 = "Loop \r\n";
  USARTInit(103);
  uart_puts(ptr1);
  _delay_ms(1000);
  // Config Settings are in lcd.h
  setup();
  uart_puts(ptr2);
  _delay_ms(1000);
  while (1) {
    loop();
    uart_puts(ptr3);
    _delay_ms(1000);
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

    //testdrawchar(buffer);
    drawstring(buffer, 0, 0, "Dan Charles Norm test test test");
    drawstring(buffer, 0, 3, "Dan Charles Norm");
    write_buffer(buffer);
}

