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

