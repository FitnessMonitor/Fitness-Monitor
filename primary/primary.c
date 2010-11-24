#include "primary.h"

uint8_t get_sample(uint8_t ADC_val)
{
	if (ADC_val = 0) init_ADC0();
	if (ADC_val = 1) init_ADC1();
	if (ADC_val = 7) init_ADC7();
	//start the conversion
	ADC_start_single_conversion();

	//wait for the ADC to finish
	while((ADCSRA & (1<<ADSC))){};
	
	//return the ADC result
	return ADCH; 
}
