#include "heart_rate.h"

extern uint8_t HR_calculate_avg(uint8_t * value, uint8_t len)
{
	uint8_t hr_avg;
	uint16_t hr_sum = 0;		
	uint8_t i;
	for (i=0; i<len; i++)
	{
		hr_sum = hr_sum + value[i];
	}
	hr_avg = hr_sum / len;
	
	return hr_avg;
}

extern uint8_t HR_rlng_avg(uint8_t value_1, uint8_t value_2, uint8_t value_3)
{
	uint16_t hr_rlng_avg;
	hr_rlng_avg = (value_1+value_2+value_3)/3;
	return hr_rlng_avg;
}

extern 
