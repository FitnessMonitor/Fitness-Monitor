#include <avr/interrupt.h>

#define ADC0 0x00
#define ADC1 0x01
#define ADC2 0x02
#define ADC3 0x03
#define ADC4 0x04
#define ADC5 0x05
#define ADC6 0x06
#define ADC7 0x07
#define ADC8 0x08

#ifndef _ADC_Header_
#define _ADC_Header_

#define ADC_enable() (ADCSRA |= (1<<ADEN))
#define ADC_disable() (ADCSRA &= ~(1<<ADEN))
#define ADC_interrupt_enable() (ADCSRA |= (1<<ADIE))
#define ADC_interrupt_disable() (ADCSRA &= ~(1<<ADIE))
#define ADC_start_conversion() (ADCSRA |= (1<<ADSC))

uint8_t get_adc_sample(uint8_t ADC_val);
extern void ADC_init();
extern void init_ADC0();
extern void init_ADC1();
extern void init_ADC7();
extern void ADC_start_single_conversion();
#endif

