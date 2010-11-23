#ifndef _ADC_H_
#define _ADC_H_

#define timer2_1ms_reset() TCNT2 = 130

extern void timer2_1ms_setup();
extern void RTC_get_dhms (volatile uint32_t * RTC_count, int16_t * days, int8_t * hours, int8_t * minutes, int8_t * seconds);
extern void RTC_set_dhms (volatile uint32_t * RTC_count, int16_t days, int8_t hours, int8_t minutes, int8_t seconds);
#endif
