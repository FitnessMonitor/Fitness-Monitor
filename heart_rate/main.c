//RF transmission
#define F_CPU 8000000UL /* 8 MHz Internal Oscillator */
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
//#include "../spi.c"
//#include "../nRF24L01.c"
#include "../sleep.c"
#include "../ADC.c"
#include "../lcd.c"
#include "../heart_rate.c"
#include "../rtc.c"



//Macros for setting, clearing and toogleing bits.
#define SET_BIT(PORT, BITNUM) ((PORT) |= (1<<(BITNUM)))
#define CLEAR_BIT(PORT, BITNUM) ((PORT) &= ~(1<<(BITNUM)))
#define TOGGLE_BIT(PORT, BITNUM) ((PORT) ^= (1<<(BITNUM)))



ISR( PCINT2_vect ) 
{
	//nRF24L01_interrupt ();
}

ISR(TIMER2_OVF_vect)	//when timer 2 interrupts
{			//wake up from sleeping
	sleep_disable();
	timer2_1ms_reset();	//reset timer to interrupt in 1ms
	hr_count++;
	btwn_beat_ctr++;	
	ms_count++;
}

ISR(ADC_vect)
{
	ADC_running = 0;
	hr_samples[sample[0]] = ADCH;
}

int main(void){
	//initialize ports
	timer2_1ms_setup();
	SET_BIT(DDRC,5);
	

	ADC_init();	//initialize ADC
	
	sample[0] = 2;	// these are the starting locations in the array of hr_samples
	sample[1] = 1;
	sample[2] = 0;
	

	lcd_init(LCD_DISP_ON);
	lcd_clrscr();
	lcd_puts("test");

	uint8_t days;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	
	char day_str[2];
	char hour_str[2];
	char min_str[2];
	char sec_str[2];

	RTC_counter = 0;


	while(1)
	{
		if (ms_count >= 1000)
		{
			RTC_counter++;
			ms_count = 0;
			RTC_get_dhms (RTC_counter, &days, &hours, &minutes, &seconds);
			
			utoa( days, day_str, 10 );
			utoa( hours, hour_str, 10 );
			utoa( minutes, min_str, 10 );
			utoa( seconds, sec_str, 10 );

			lcd_clrscr();

			lcd_putc(day_str[0]);
			lcd_putc(day_str[1]);
			lcd_puts(":");
			lcd_putc(hour_str[0]);
			lcd_putc(hour_str[1]);
			lcd_puts(":");
			lcd_putc(min_str[0]);
			lcd_putc(min_str[1]);
			lcd_puts(":");
			lcd_putc(sec_str[0]);
			lcd_putc(sec_str[1]);
			
		}

		if (hr_count >= 100)
		{
			//lcd_clrscr();
			//clear the counter
			hr_count = 0;
	
			//start an ADC
			ADC_start_single_conversion();
			
			uint8_t i;
			//update locations
			for (i = 0; i<3; i++)
			{
				sample[i]++;
				if (sample[i] >=40)
				{
					sample[i] = 0;	
				}
			}
			if (interval >= 20)
			{
				interval = 0;
			}

			//wait for ADC to finish conversion
			while (ADC_running == 1)
			{
				//wait for ADC value
			}
			
			//calculate the hr_average
			hr_sum = 0;		//clear the sum
			for (i=0; i<40; i++)
			{
				hr_sum = hr_sum + hr_samples[i];
			}
			hr_avg = hr_sum / 40;
			
			//calculate the rolling average value
			hr_rlng_avg = (hr_samples[sample[0]]+hr_samples[sample[1]]+hr_samples[sample[2]])/3;
			
			//determine if high portion of beat has started
			if (( hr_rlng_avg > hr_avg )&&(beat_high == 0))
			{
				beat_high = 1;
				SET_BIT(PORTC, 5);
				hr_interval[interval] = btwn_beat_ctr;	//store the time between beats
				btwn_beat_ctr = 0; //reset the counter
			}
			else if (( hr_rlng_avg < hr_avg ) && (beat_high == 1))
			{
				beat_high = 0;
				CLEAR_BIT(PORTC,5);
			}

			//calculate average heart rate
			interval_sum = 0;
			for (i = 0; i<20; i++)
			{
				interval_sum = interval_sum + hr_interval[i];
			}
			hr_frequency = (1200000/interval_sum);
			
			utoa(hr_frequency, frequency, 10 );
			//lcd_clrscr();
			//lcd_puts("heart_rate:\n");
			for (i = 0; i<3; i++)
			{
				//lcd_putc(frequency[i]);
			}
		}

		//go back to sleep now
		sleep_now();	// sleep for 1 ms
	}
}






















