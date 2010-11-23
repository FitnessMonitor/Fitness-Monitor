<<<<<<< HEAD
#define LCD_LINES           2     // number of visible lines of the display 
#define LCD_DISP_LENGTH    16     // visibles characters per line of the display 
#define LCD_LINE_LENGTH  0x40     // internal line length of the display
#define LCD_START_LINE1  0x00     // DDRAM address of first char of line 1 
#define LCD_START_LINE2  0x40     // DDRAM address of first char of line 2 

#define LCD_PORT         PORTD        // port for the LCD lines
#define LCD_DATA0_PORT   LCD_PORT     // port for 4bit data bit 0
#define LCD_DATA1_PORT   LCD_PORT     // port for 4bit data bit 1
#define LCD_DATA2_PORT   LCD_PORT     // port for 4bit data bit 2
#define LCD_DATA3_PORT   LCD_PORT     // port for 4bit data bit 3
#define LCD_DATA0_PIN    0            // pin for 4bit data bit 0 
#define LCD_DATA1_PIN    1            // pin for 4bit data bit 1 
#define LCD_DATA2_PIN    2            // pin for 4bit data bit 2 
#define LCD_DATA3_PIN    3            // pin for 4bit data bit 3 
#define LCD_RS_PORT      LCD_PORT     // port for RS line        
#define LCD_RS_PIN       4            // pin  for RS line        
#define LCD_RW_PORT      LCD_PORT     // port for RW line        
#define LCD_RW_PIN       5            // pin  for RW line        
#define LCD_E_PORT       LCD_PORT     // port for Enable line    
#define LCD_E_PIN        6            // pin  for Enable line    

/* instruction register bit positions, see HD44780U data sheet */
#define LCD_CLR               0      // DB0: clear display                  
#define LCD_HOME              1      // DB1: return to home position        
#define LCD_ENTRY_MODE        2      // DB2: set entry mode                 
#define LCD_ENTRY_INC         1      // DB1: 1=increment, 0=decrement     
#define LCD_ENTRY_SHIFT       0      // DB2: 1=display shift on           
#define LCD_ON                3      // DB3: turn lcd/cursor on           
#define LCD_ON_DISPLAY        2      // DB2: turn display on                        
#define LCD_FUNCTION          5      // DB5: function set               
#define LCD_FUNCTION_2LINES   3      // DB3: two lines (0->one line)       
#define LCD_CGRAM             6      // DB6: set CG RAM address           
#define LCD_DDRAM             7      // DB7: set DD RAM address           
#define LCD_BUSY              7      // DB7: LCD is busy                  

/* display on/off, cursor on/off, blinking char at cursor position */
#define LCD_DISP_OFF             0x08   // display off                            
#define LCD_DISP_ON              0x0C   // display on, cursor off                                       

/* function set: set interface data length and number of display lines */
#define LCD_FUNCTION_4BIT_1LINE  0x20   // 4-bit interface, single line, 5x7 dots 
#define LCD_FUNCTION_4BIT_2LINES 0x28   // 4-bit interface, dual line,   5x7 dots 

#define LCD_MODE_DEFAULT     ((1<<LCD_ENTRY_MODE) | (1<<LCD_ENTRY_INC) )

extern void lcd_init(uint8_t dispAttr);

extern void lcd_clrscr(void);

extern void lcd_putc(char c);

extern void lcd_puts(const char *s);

extern void lcd_command(uint8_t cmd);

=======
#define LCDWIDTH 128
#define LCDHEIGHT 64

#define swap(a, b) { uint8_t t = a; a = b; b = t; }

#define CMD_DISPLAY_OFF   0xAE
#define CMD_DISPLAY_ON    0xAF

#define CMD_SET_DISP_START_LINE  0x40
#define CMD_SET_PAGE  0xB0

#define CMD_SET_COLUMN_UPPER  0x10
#define CMD_SET_COLUMN_LOWER  0x00

#define CMD_SET_ADC_NORMAL  0xA0
#define CMD_SET_ADC_REVERSE 0xA1

#define CMD_SET_DISP_NORMAL 0xA6
#define CMD_SET_DISP_REVERSE 0xA7

#define CMD_SET_ALLPTS_NORMAL 0xA4
#define CMD_SET_ALLPTS_ON  0xA5
#define CMD_SET_BIAS_9 0xA2 
#define CMD_SET_BIAS_7 0xA3

#define CMD_RMW  0xE0
#define CMD_RMW_CLEAR 0xEE
#define CMD_INTERNAL_RESET  0xE2
#define CMD_SET_COM_NORMAL  0xC0
#define CMD_SET_COM_REVERSE  0xC8
#define CMD_SET_POWER_CONTROL  0x28
#define CMD_SET_RESISTOR_RATIO  0x20
#define CMD_SET_VOLUME_FIRST  0x81
#define  CMD_SET_VOLUME_SECOND  0
#define CMD_SET_STATIC_OFF  0xAC
#define  CMD_SET_STATIC_ON  0xAD
#define CMD_SET_STATIC_REG  0x0
#define CMD_SET_BOOSTER_FIRST  0xF8
#define CMD_SET_BOOSTER_234  0
#define  CMD_SET_BOOSTER_5  1
#define  CMD_SET_BOOSTER_6  3
#define CMD_NOP  0xE3
#define CMD_TEST  0xF0

// Backlight
#define BLA_DDR DDRC
#define BLA_PIN PINC
#define BLA_PORT PORTC
#define BLA 3

// SDA (SI)
#define SID_DDR DDRD
#define SID_PIN PIND
#define SID_PORT PORTD
#define SID 4

// SCK
#define SCLK_DDR DDRD
#define SCLK_PIN PIND
#define SCLK_PORT PORTD
#define SCLK 3

// A0
#define A0_DDR DDRD
#define A0_PIN PIND
#define A0_PORT PORTD
#define A0 2

// RST
#define RST_DDR DDRC
#define RST_PIN PINC
#define RST_PORT PORTC
#define RST 5

// CS1B
#define CS_DDR DDRC
#define CS_PIN PINC
#define CS_PORT PORTC
#define CS 4

#define LED_DDR DDRC
#define LED_PORT PORTC
#define LED 3

void spiwrite(uint8_t c);

void setup(void);
void loop(void);

void st7565_init(void);
void st7565_command(uint8_t c);
void st7565_data(uint8_t c);
void st7565_set_brightness(uint8_t val);

void clear_screen(void);
void clear_buffer(uint8_t *buffer);
void write_buffer(uint8_t *buffer);

void setpixel(uint8_t *buff, uint8_t x, uint8_t y, uint8_t color);

void drawbitmap(uint8_t *buff, uint8_t x, uint8_t y, 
		const uint8_t bitmap, uint8_t w, uint8_t h,
		uint8_t color);
void drawchar(uint8_t *buff, uint8_t x, uint8_t line, uint8_t c);

>>>>>>> 5793541bd196650923ae10c25b167ddf9128cc26
