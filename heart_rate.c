#include "heart_rate.h"

volatile uint8_t hr_samples[40];	//ADC sample values	
volatile uint16_t hr_interval[20];	//time between beats
volatile uint8_t interval;	//location in array of interval times
volatile uint8_t sample[3];	//location in array of hr samples

uint16_t hr_sum;
uint16_t hr_avg;
uint16_t hr_rlng_avg;
uint32_t interval_sum;
uint8_t hr_frequency;

volatile uint8_t beat_high;
char frequency[3];

volatile uint8_t hr_count;

volatile uint16_t btwn_beat_ctr;	//stores time between beats


extern uint8_t HR_calculate_avg(uint8_t * value, uint8_t * len)
{
	uint8_t hr_avg;
	uint16_t hr_sum = 0;		//clear the sum
	uint8_t i;
	for (i=0; i<40; i++)
	{
		hr_sum = hr_sum + value[i];
	}
	hr_avg = hr_sum / 40;
	
	return hr_avg;
}

extern uint8_t HR_rlng_avg(uint8_t value_1, uint8_t value_2, uint8_t value_3)
{
	uint16_t hr_rlng_avg;
	hr_rlng_avg = (value_1+value_2+value_3)/3;
	return hr_rlng_avg;
}

extern 
