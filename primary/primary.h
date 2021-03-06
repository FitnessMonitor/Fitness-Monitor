#ifndef _primary_header_
#define _primary_hader_

//Macros for setting, clearing and toogleing bits.
#define SET_BIT(PORT, BITNUM) ((PORT) |= (1<<(BITNUM)))
#define CLEAR_BIT(PORT, BITNUM) ((PORT) &= ~(1<<(BITNUM)))
#define TOGGLE_BIT(PORT, BITNUM) ((PORT) ^= (1<<(BITNUM)))

#define backlight_on PORTC |= 1<<PC2
#define backlight_off PORTC &= ~(1<<PC2)

static void IoInit ();
void setup(); 
void disp_hms(uint8_t hours, uint8_t minutes, uint8_t seconds);
void i2s(int i,char *s);
int init_sdcard(void);
int sdcard_open(uint8_t hours, uint8_t minutes, uint8_t seconds);
void sdcard_close();
void get_steps(uint8_t *points, uint8_t size, uint16_t * avg, uint16_t * steps, uint16_t * activity_level);

//test
#endif


