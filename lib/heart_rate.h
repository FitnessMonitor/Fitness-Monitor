#ifndef _heart_rate_header_
#define _heart_rate_hader_

<<<<<<< HEAD
extern uint8_t HR_calculate_avg(uint8_t * value, uint8_t len);
extern uint8_t HR_rlng_avg(uint8_t value_1, uint8_t value_2, uint8_t value_3);
=======
#define hr_threshold 90
void init_secondary_device (void);
uint8_t get_hr_sample(void);
uint16_t calculate_bpm(uint16_t *samples, uint16_t len);
>>>>>>> 5793541bd196650923ae10c25b167ddf9128cc26

#endif

