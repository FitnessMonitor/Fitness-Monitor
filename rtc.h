#ifndef _ADC_H_
#define _ADC_H_

#define timer2_1ms_reset() TCNT2 = 247

extern void timer2_1ms_setup();
extern void update_time();
#endif
