#include <avr/sleep.h>
#include <avr/interrupt.h>
#ifndef _sleep_H_
#define _sleep_H_
void sleep_init();
extern void sleep_now();
#endif

