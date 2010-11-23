#include "ADC.h"

<<<<<<< HEAD
#define ADC0 0x00
#define ADC1 0x01
#define ADC2 0x02
#define ADC3 0x03
#define ADC4 0x04
#define ADC5 0x05
#define ADC6 0x06
#define ADC7 0x07
#define ADC8 0x08

=======
>>>>>>> 5793541bd196650923ae10c25b167ddf9128cc26
extern void ADC_init(int ADC_channel)	//initialize ADC
{
	//bring ADC out of low power state
	ADCSRA |= (0 << ADPS2) | (0 << ADPS1) | (1 << ADPS0);  //set ADC clock prescaler to 2

<<<<<<< HEAD
	ADMUX |= (1 << REFS1) | (1 << REFS0); // AVCC as reference with external capacitor at AREF pin
=======
	ADMUX |= (0 << REFS1) | (1 << REFS0); // AVCC as reference with external capacitor at AREF pin
>>>>>>> 5793541bd196650923ae10c25b167ddf9128cc26
	ADMUX |= (1 << ADLAR) | ADC_channel; // Left adjust ADC result to allow easy 8 bit reading

	DIDR0 =0;
	DIDR0 |= (1<<ADC_channel);


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




<<<<<<< HEAD
=======




>>>>>>> 5793541bd196650923ae10c25b167ddf9128cc26