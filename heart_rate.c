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


/*
		if(hr_counter >=hr_sample_rate)
		{
			hr_counter = 0;
			hr_samples[sample] = 0;

			ADC_start_single_conversion();
			
			// wait for the ADC to finish
			while((ADCSRA & (1<<ADSC))){};

			hr_samples[sample] = ADCL;
			hr_samples[sample] += (ADCH << 8); 
			
			hr_sum = 0;		
			for (i=0; i<t_hr_samples; i++)
			{
				//add up the samples
				hr_sum = hr_sum + hr_samples[i];

			}
			avg_hr_val = hr_sum / t_hr_samples;


			if ((hr_samples[sample] > (avg_hr_val+10) ) && (beat_started == 0))
			{
				beat_started = 1;
				btwn_beats[beat] = btwn_bt_ctr;
				btwn_bt_ctr = 0;
				SET_BIT(PORTC, 5);
			}
			else if ((hr_samples[sample] < avg_hr_val) && (beat_started == 1) && (btwn_bt_ctr > 300))
			{
				beat_started = 0;
				CLEAR_BIT(PORTC,5);
			}

			
			if ((hr_max_val-hr_min_val)<5)
			{
			//	lcd_puts("woops");
			}

			for (i=0; i<3; i++)
			{
				sample++;
				if (sample >= t_hr_samples) sample = 0;
			}
			beat++;
			if (beat >=beats_to_avg) beat = 0;

		}	
*/
