<<<<<<< HEAD
#include "lcd.h"

#define DDR(x) (*(&x - 1))      /* address of data direction register of port x */
#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)
    /* on ATmega64/128 PINF is on port 0x00 and not 0x60 */
    #define PIN(x) ( &PORTF==&(x) ? _SFR_IO8(0x00) : (*(&x - 2)) )
#else
	#define PIN(x) (*(&x - 2))    /* address of input register of port x          */
#endif

#define lcd_e_delay()   __asm__ __volatile__( "rjmp 1f\n 1:" );
#define lcd_e_high()    LCD_E_PORT  |=  _BV(LCD_E_PIN);
#define lcd_e_low()     LCD_E_PORT  &= ~_BV(LCD_E_PIN);
#define lcd_e_toggle()  toggle_e()
#define lcd_rw_high()   LCD_RW_PORT |=  _BV(LCD_RW_PIN)
#define lcd_rw_low()    LCD_RW_PORT &= ~_BV(LCD_RW_PIN)
#define lcd_rs_high()   LCD_RS_PORT |=  _BV(LCD_RS_PIN)
#define lcd_rs_low()    LCD_RS_PORT &= ~_BV(LCD_RS_PIN)
#define LCD_FUNCTION_DEFAULT    LCD_FUNCTION_4BIT_2LINES 

/* 
** function prototypes 
*/
static void toggle_e(void);

/*
** local functions
*/

/* toggle Enable Pin to initiate write */
static void toggle_e(void)
{
    lcd_e_high();
    lcd_e_delay();
    lcd_e_low();
}

static void lcd_write(uint8_t data,uint8_t rs) 
{
    unsigned char dataBits ;

    if (rs) {   /* write data        (RS=1, RW=0) */
       lcd_rs_high();
    } else {    /* write instruction (RS=0, RW=0) */
       lcd_rs_low();
    }
    lcd_rw_low();

    /* configure data pins as output */
    DDR(LCD_DATA0_PORT) |= 0x0F;

    /* output high nibble first */
    dataBits = LCD_DATA0_PORT & 0xF0;
    LCD_DATA0_PORT = dataBits |((data>>4)&0x0F);
    lcd_e_toggle();

    /* output low nibble */
    LCD_DATA0_PORT = dataBits | (data&0x0F);
    lcd_e_toggle();
 
    /* all data pins high (inactive) */
    LCD_DATA0_PORT = dataBits | 0x0F;

}

static uint8_t lcd_read(uint8_t rs) 
{
    uint8_t data;
    
    
    if (rs)
        lcd_rs_high();                       /* RS=1: read data      */
    else
        lcd_rs_low();                        /* RS=0: read busy flag */
    lcd_rw_high();                           /* RW=1  read mode      */

    DDR(LCD_DATA0_PORT) &= 0xF0;         /* configure data pins as input */
        
    lcd_e_high();
    lcd_e_delay();        
    data = PIN(LCD_DATA0_PORT) << 4;     /* read high nibble first */
    lcd_e_low();
        
    lcd_e_delay();                       /* Enable 500ns low       */
        
    lcd_e_high();
    lcd_e_delay();
    data |= PIN(LCD_DATA0_PORT)&0x0F;    /* read low nibble        */
    lcd_e_low();
    
    return data;
}


/*************************************************************************
loops while lcd is busy, returns address counter
*************************************************************************/
static uint8_t lcd_waitbusy(void)

{
    register uint8_t c;
    
    /* wait until busy flag is cleared */
    while ( (c=lcd_read(0)) & (1<<LCD_BUSY)) {}
    
    /* the address counter is updated 4us after the busy flag is cleared */
    _delay_us(2);

    /* now read the address counter */
    return (lcd_read(0));  // return address counter
    
}/* lcd_waitbusy */


/*************************************************************************
Move cursor to the start of next line or to the first line if the cursor 
is already on the last line.
*************************************************************************/
static inline void lcd_newline(uint8_t pos)
{
    register uint8_t addressCounter;

    if ( pos < (LCD_START_LINE2) )
        addressCounter = LCD_START_LINE2;
    else
        addressCounter = LCD_START_LINE1;
    lcd_command((1<<LCD_DDRAM)+addressCounter);

}/* lcd_newline */

/*
** PUBLIC FUNCTIONS 
*/

//Send LCD controller instruction command
void lcd_command(uint8_t cmd)
{
    lcd_waitbusy();
    lcd_write(cmd,0);
}

//Clear display and set cursor to home position
void lcd_clrscr(void)
{
    lcd_command(1<<LCD_CLR);
}

//Display character at current cursor position 
void lcd_putc(char c)
{
    uint8_t pos;
    pos = lcd_waitbusy();   // read busy-flag and address counter
    if (c=='\n'){lcd_newline(pos);}
    else{lcd_write(c, 1);}
}

//Display string 
void lcd_puts(const char *s)
{
    register char c;
    while ( (c = *s++) ) {
        lcd_putc(c);
    }

}

/*************************************************************************
Initialize display and select type of cursor 
Input:    dispAttr LCD_DISP_OFF            display off
                   LCD_DISP_ON             display on, cursor off
                   LCD_DISP_ON_CURSOR      display on, cursor on
                   LCD_DISP_CURSOR_BLINK   display on, cursor on flashing
Returns:  none
*************************************************************************/
void lcd_init(uint8_t dispAttr)  //n
{
    /* configure all port bits as output (all LCD lines on same port) */
    DDR(LCD_DATA0_PORT) |= 0x7F;    
    _delay_us(16000);        /* wait 16ms or more after power-on       */
    
    /* initial write to lcd is 8bit */
    LCD_DATA1_PORT |= _BV(LCD_DATA1_PIN);  // _BV(LCD_FUNCTION)>>4;
    LCD_DATA0_PORT |= _BV(LCD_DATA0_PIN);  // _BV(LCD_FUNCTION_8BIT)>>4;
    lcd_e_toggle();
    _delay_us(4992);         /* delay, busy flag can't be checked here */
   
    /* repeat last command */ 
    lcd_e_toggle();      
    _delay_us(64);           /* delay, busy flag can't be checked here */
    
    /* repeat last command a third time */
    lcd_e_toggle();      
    _delay_us(64);           /* delay, busy flag can't be checked here */

    /* now configure for 4bit mode */
    LCD_DATA0_PORT &= ~_BV(LCD_DATA0_PIN);   // LCD_FUNCTION_4BIT_1LINE>>4
    lcd_e_toggle();
    _delay_us(64);           /* some displays need this additional delay */
    
    /* from now the LCD only accepts 4 bit I/O, we can use lcd_command() */    

    lcd_command(LCD_FUNCTION_DEFAULT);      /* function set: display lines  */
    lcd_command(LCD_DISP_OFF);              /* display off                  */
    lcd_clrscr();                           /* display clear                */ 
    lcd_command(LCD_MODE_DEFAULT);          /* set entry mode               */
    lcd_command(dispAttr);                  /* display/cursor control       */

}/* lcd_init */
=======
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

>>>>>>> 5793541bd196650923ae10c25b167ddf9128cc26
