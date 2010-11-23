#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h> 
#include <string.h>
#include "lcd.h"

extern uint8_t PROGMEM font[];
int pagemap[] = { 3, 2, 1, 0, 7, 6, 5, 4 };

void drawstring(uint8_t *buff, uint8_t x, uint8_t line, char *c) {
  while (c[0] != 0) {
    drawchar(buff, x, line, c[0]);
    c++;
    x += 6; // 6 pixels wide
    if (x + 6 >= LCDWIDTH) {
      x = 0;    // ran out of this line
      line++;
    }
    if (line >= (LCDHEIGHT/8))
      return;        // ran out of space :(
  }

}

void drawchar(uint8_t *buff, uint8_t x, uint8_t line, uint8_t c) {
  uint8_t i =0;
  for ( i =0; i<5; i++ ) {
    buff[x + (line*128) ] = pgm_read_byte(font+(c*5)+i);
    x++;
  }
}

void clear_screen(void) {
  uint8_t p, c;
  for(p = 0; p < 8; p++) {
    st7565_command(CMD_SET_PAGE | p);
    for(c = 0; c < 129; c++) {
      st7565_command(CMD_SET_COLUMN_LOWER | (c & 0xf));
      st7565_command(CMD_SET_COLUMN_UPPER | ((c >> 4) & 0xf));
      st7565_data(0x0);
    }     
  }
}

// clear everything
void clear_buffer(uint8_t *buff) {
  memset(buff, 0, 512);
}


void st7565_init(void) {
  // set pin directions
  SID_DDR |= _BV(SID);
  SCLK_DDR |= _BV(SCLK);
  A0_DDR |= _BV(A0);
  RST_DDR |= _BV(RST);
  CS_DDR |= _BV(CS);
  
  // toggle RST low to reset; CS low so it'll listen to us
  CS_PORT &= ~_BV(CS);
  RST_PORT &= ~_BV(RST);
  _delay_ms(500);
  RST_PORT |= _BV(RST);

  // LCD bias select
  st7565_command(CMD_SET_BIAS_7);
  // ADC select
  st7565_command(CMD_SET_ADC_NORMAL);
  // SHL select
  st7565_command(CMD_SET_COM_NORMAL);

  // black text on white background
  st7565_command(CMD_SET_DISP_REVERSE);

  // Initial display line
  st7565_command(CMD_SET_DISP_START_LINE);

  // turn on voltage converter (VC=1, VR=0, VF=0)
  st7565_command(CMD_SET_POWER_CONTROL | 0x4);
  _delay_ms(50);

  // turn on voltage regulator (VC=1, VR=1, VF=0)
  st7565_command(CMD_SET_POWER_CONTROL | 0x6);
  _delay_ms(50);

  // turn on voltage follower (VC=1, VR=1, VF=1)
  st7565_command(CMD_SET_POWER_CONTROL | 0x7);
  _delay_ms(10);

  // set lcd operating voltage (regulator resistor, ref voltage resistor)
  st7565_command(CMD_SET_RESISTOR_RATIO | 0x2);

  //set brightness
  st7565_set_brightness(0x15);

}

inline void spiwrite(uint8_t c) {
  int8_t i;
  for (i=7; i>=0; i--) {
    SCLK_PORT &= ~_BV(SCLK);
    if (c & _BV(i))
      SID_PORT |= _BV(SID);
    else
      SID_PORT &= ~_BV(SID);
    SCLK_PORT |= _BV(SCLK);
  }

}
void st7565_command(uint8_t c) {
  A0_PORT &= ~_BV(A0);
  spiwrite(c);
}

void st7565_data(uint8_t c) {
  A0_PORT |= _BV(A0);
  spiwrite(c);
}

void st7565_set_brightness(uint8_t val) {
    st7565_command(CMD_SET_VOLUME_FIRST);
    st7565_command(CMD_SET_VOLUME_SECOND | (val & 0x3f));
}


void write_buffer(uint8_t *buffer) {
  uint8_t c, p;

  for(p = 0; p < 8; p++) {
    st7565_command(CMD_SET_PAGE | pagemap[p]);
    st7565_command(CMD_SET_COLUMN_LOWER | (0x0 & 0xf));
    st7565_command(CMD_SET_COLUMN_UPPER | ((0x0 >> 4) & 0xf));
    st7565_command(CMD_RMW);
    st7565_data(0xff);

    for(c = 0; c < 128; c++) {
      st7565_data(buffer[(128*p)+c]);
    }
  }
}

