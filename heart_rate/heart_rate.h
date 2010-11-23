#ifndef _heart_rate_header_
#define _heart_rate_hader_

//Macros for setting, clearing and toogleing bits.
#define SET_BIT(PORT, BITNUM) ((PORT) |= (1<<(BITNUM)))
#define CLEAR_BIT(PORT, BITNUM) ((PORT) &= ~(1<<(BITNUM)))
#define TOGGLE_BIT(PORT, BITNUM) ((PORT) ^= (1<<(BITNUM)))

#define hr_threshold 90
void init_secondary_device (void);
uint8_t get_hr_sample(void);
uint16_t calculate_bpm(uint16_t *samples, uint16_t len);

#endif

