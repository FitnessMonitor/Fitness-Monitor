#include "heart_rate.h"

<<<<<<< HEAD
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

=======
void init_secondary_device (void)
{
	//initialize ports
	DDRD |= 1<<PD2;

	// Initialize AVR for use with nRF24L01
	nRF24L01_init();
	// Wait for nRF24L01 to come up
	_delay_ms(50);

	//initialize timer 2 to interrupt ever 1ms
	timer2_1ms_setup();

	// Configure nRF24L01
	nRF24L01_config();

	//setup UART
	//USARTInit(MYUBRR);

	//initialize ADC
	ADC_init(ADC0);
}

uint8_t get_hr_sample(void)
{
	//start the conversion
	ADC_start_single_conversion();

	//wait for the ADC to finish
	while((ADCSRA & (1<<ADSC))){};
	
	//return the ADC result
	return ADCH; 
}

uint16_t calculate_bpm(uint16_t *samples, uint16_t len)
{
	uint16_t sum = 0;
	uint16_t interval = 0;
	uint16_t beats = 0;
	uint16_t index = 0;
	uint16_t last_beat = 0;
	uint16_t avg = 0;
	uint16_t bpm = 0;

	while ((samples[index] <= hr_threshold) && (index < len)) index++;
	last_beat = index;

	while (index < len)
	{
		if (samples[index] >=80)
		{
			last_beat = index;
			//test if it is an acceptable interval
			interval = last_beat - index;
			if ( (interval >=6 ) && (interval <=30 ) )	// filter for 40 to 200bpm
			{
				sum = sum+interval;
				beats++;
			}
		}
		index++;
	}
	avg = (50*sum)/beats;
	bpm = 60000/avg;	
	
	return bpm; 
}


















>>>>>>> 5793541bd196650923ae10c25b167ddf9128cc26
