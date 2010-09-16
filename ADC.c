#include "ADC.h"


extern void ADC_init()	//initialize ADC
{
   ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC prescaler to 128 - 125KHz sample rate @ 16MHz

   ADMUX |= (1 << REFS0); // Set ADC reference to AVCC
   ADMUX |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading

   // No MUX values needed to be changed to use ADC0
}





