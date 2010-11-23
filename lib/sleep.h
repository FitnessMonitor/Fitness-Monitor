#include <avr/sleep.h>
#include <avr/interrupt.h>
#ifndef _sleep_H_
#define _sleep_H_

#define timer2_1ms_reset() TCNT2 = 130

void sleep_init();
extern void sleep_now();
extern void timer2_1ms_setup();
#endif

