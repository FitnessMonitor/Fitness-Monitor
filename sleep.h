#include <avr/sleep.h>
#include <avr/interrupt.h>
#ifndef _sleep_H_
#define _sleep_H_
void sleep_init();
void sleep_timer_destroy();
extern void sleep_int_handler();
extern void sleep_1_ms( uint16_t loops);
#endif

