#ifndef _primary_header_
#define _primary_hader_

//Macros for setting, clearing and toogleing bits.
#define SET_BIT(PORT, BITNUM) ((PORT) |= (1<<(BITNUM)))
#define CLEAR_BIT(PORT, BITNUM) ((PORT) &= ~(1<<(BITNUM)))
#define TOGGLE_BIT(PORT, BITNUM) ((PORT) ^= (1<<(BITNUM)))


static void IoInit ();
void setup(); 
void disp_hms(uint8_t hours, uint8_t minutes, uint8_t seconds);
void i2s(int i,char *s);
int init_sdcard(void);
int sdcard_open(uint8_t *name);
void sdcard_close();


#endif


