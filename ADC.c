#include "ADC.h"

extern void ADC_init(int ADC_channel)	//initialize ADC
{
	//bring ADC out of low power state
	ADCSRA |= (0 << ADPS2) | (0 << ADPS1) | (1 << ADPS0);  //set ADC clock prescaler to 2

	ADMUX |= (0 << REFS1) | (1 << REFS0); // AVCC as reference with external capacitor at AREF pin
	ADMUX |= (1 << ADLAR) | ADC_channel; // Left adjust ADC result to allow easy 8 bit reading

	DIDR0 =0;
	DIDR0 |= (1<<ADC_channel);


	// No MUX values needed to be changed to use ADC0
}

extern void ADC_init_ADC7(void)	//initialize ADC
{
	//bring ADC out of low power state
	ADCSRA |= (0 << ADPS2) | (0 << ADPS1) | (1 << ADPS0);  //set ADC clock prescaler to 2

	ADMUX |= (0 << REFS1) | (1 << REFS0); 	// AVCC as reference with external capacitor at AREF pin
	ADMUX |= (1 << ADLAR);			 // Left adjust ADC result to allow easy 8 bit reading
	ADMUX |= (0 << MUX3) | (1 << MUX2) |(1 << MUX1) | (1 << MUX0); //select ADC7

	//DIDR0 =0;
	//DIDR0 |= (1<<ADC_channel);


	// No MUX values needed to be changed to use ADC0
}

extern void ADC_start_single_conversion()
{
	//start an ADC
	ADC_enable();	
	ADC_interrupt_enable();
	//ADC_running = 1;
	ADC_start_conversion();
}

extern void ADC_shutdown()
{
	ADC_disable();
}




