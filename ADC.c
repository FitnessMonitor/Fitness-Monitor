#include "ADC.h"


// 0, 1, 7
extern void ADC_init()	//initialize ADC
{
	//bring ADC out of low power state
	ADCSRA |= (0 << ADPS2) | (0 << ADPS1) | (1 << ADPS0);  //set ADC clock prescaler to 2

	ADMUX |= (0 << REFS1) | (1 << REFS0); // AVCC as reference with external capacitor at AREF pin
	ADMUX |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading

}

extern void init_ADC0()
{
	ADC_init();
	ADMUX |= (0 << MUX3) | (0 << MUX2 ) | ( 0 << MUX1 ) | ( 0 << MUX0); //use ADC0
	DIDR0 |= (1<<ADC0D);  //disable digital input
}

extern void init_ADC1()
{
	ADC_init();
	ADMUX |= (0 << MUX3) | (0 << MUX2 ) | ( 0 << MUX1 ) | ( 1 << MUX0); //use ADC1
	DIDR0 |= (1<<ADC1D); //disable digital input
}

extern void init_ADC7()
{
	ADC_init();
	ADMUX |= (0 << MUX3) | (1 << MUX2 ) | ( 1 << MUX1 ) | ( 1 << MUX0); //use ADC7
	//ADC7 is an analog only pin (no need to disable digital input)
}

extern void ADC_start_single_conversion()
{
	//start an ADC
	ADC_enable();	
	ADC_interrupt_enable();
	ADC_start_conversion();
}

extern void ADC_shutdown()
{
	ADC_disable();
}








