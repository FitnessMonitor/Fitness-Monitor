#include <avr/interrupt.h>

#ifndef _ADC_Header_
#define _ADC_Header_

#define ADC_enable() (ADCSRA |= (1<<ADEN))
#define ADC_disable() (ADCSRA &= ~(1<<ADEN))
#define ADC_interrupt_enable() (ADCSRA |= (1<<ADIE))
#define ADC_interrupt_disable() (ADCSRA &= ~(1<<ADIE))
#define ADC_start_conversion() (ADCSRA |= (1<<ADSC))

extern void ADC_init();
#endif

