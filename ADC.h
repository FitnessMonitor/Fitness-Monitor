#include <avr/sleep.h>
#include <avr/interrupt.h>

#ifndef _ADC_H_
#define _ADC_H_
#define SET_BIT(PORT, BITNUM) ((PORT) |= (1<<(BITNUM)))
#define ADC_enable() (ADCSRA |= (1<<ADEN))
#define ADC_disable() (ADCSRA &= ~(1<<ADEN))
#define ADC_interrupt_enable() (ADCSRA |= (1<<ADIE))
#define ADC_interrupt_disable() (ADCSRA &= ~(1<<ADIE))
#define ADC_start_conversion() (ADCSRA |= (1<<ADSC))
extern void ADC_init();
#endif

