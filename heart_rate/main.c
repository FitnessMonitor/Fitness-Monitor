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
#include "../rtc.c"
//#include "../heart_rate.c"

//Macros for setting, clearing and toogleing bits.
#define SET_BIT(PORT, BITNUM) ((PORT) |= (1<<(BITNUM)))
#define CLEAR_BIT(PORT, BITNUM) ((PORT) &= ~(1<<(BITNUM)))
#define TOGGLE_BIT(PORT, BITNUM) ((PORT) ^= (1<<(BITNUM)))


#define hr_sample_rate 100  //sample every 100ms
#define t_hr_samples 40 //(40000 / hr_sample_rate)
#define beats_to_avg 10 //(t_hr_samples / 4)
#define starting_avg_hr_val 90
#define starting_avg_hr 1000

volatile uint32_t RTC_sec_counter;
volatile uint16_t RTC_ms_counter;

volatile uint8_t hr_samples[t_hr_samples+1];
volatile uint8_t sample[3] = {0, 1, 2};
volatile uint8_t hr_counter;

volatile uint16_t btwn_bt_ctr;


ISR( PCINT2_vect ) 
{
	//nRF24L01_interrupt ();
}

ISR(TIMER2_OVF_vect)	//when timer 2 interrupts
{			//wake up from sleeping
	sleep_disable();
	timer2_1ms_reset();	//reset timer to interrupt in 1ms
	RTC_ms_counter++;
	hr_counter++;
	btwn_bt_ctr++;
	
}

ISR(ADC_vect)
{
	hr_samples[sample[0]] = ADCH;
}

int main(void){

	//initialize ports
	SET_BIT(DDRC,5);
	
	//initialize ADC
	ADC_init();
	
	//initialize LCD screen
	lcd_init(LCD_DISP_ON);
	lcd_clrscr();
	//lcd_puts("test");

	RTC_sec_counter = 0;
	
	int16_t days = 0;
	int8_t hours = 0;
	int8_t minutes = 0;
	int8_t seconds = 0;

	timer2_1ms_setup();


	char sec_str[2];
	char min_str[2];
	char hr_str[2];
	char day_str[3];
	RTC_set_dhms (&RTC_sec_counter, days, hours, minutes, seconds);

	uint16_t hr_sum = 0;
	uint8_t avg_hr_val = 0;
	uint8_t hr_min_val = 0;
	uint8_t hr_max_val = 0;
	uint8_t hr_rlng_avg;
	uint8_t beat_started = 0;
	uint16_t btwn_beats[50];
	uint8_t beat = 0;
	uint32_t time_sum = 0;
	uint16_t avg_beat_time;
	uint16_t avg_hr = 0;
	char avg_hr_str[5];
	

	uint8_t i;



	for (i=0; i<t_hr_samples; i++)
	{
		hr_samples[i] = starting_avg_hr_val;
		btwn_beats[i] = starting_avg_hr;
	}


	while(1)
	{
		if(RTC_ms_counter >= 1000)
		{
			RTC_sec_counter++;
			RTC_get_dhms (&RTC_sec_counter, &days, &hours, &minutes, &seconds);
			lcd_puts("m");
			itoa(seconds, sec_str,10);
			itoa(minutes, min_str,10);
			itoa(hours, hr_str,10);
			itoa(days, day_str,10);
			ultoa(avg_hr, avg_hr_str,10);
			lcd_clrscr();
			lcd_putc(day_str[0]);
			lcd_putc(day_str[1]);
			lcd_puts(":");			
			lcd_putc(hr_str[0]);
			lcd_putc(hr_str[1]);
			lcd_puts(":");
			lcd_putc(min_str[0]);
			lcd_putc(min_str[1]);
			lcd_puts(":");
			lcd_putc(sec_str[0]);
			lcd_putc(sec_str[1]);
			lcd_puts("\n");
			lcd_putc(avg_hr_str[0]);
			lcd_putc(avg_hr_str[1]);
			lcd_putc(avg_hr_str[2]);
			lcd_putc(avg_hr_str[3]);
			lcd_putc(avg_hr_str[4]);
			RTC_ms_counter = 0;
		}

		if(hr_counter >=50)
		{
			hr_counter = 0;
			hr_samples[sample[0]] = 0;

			ADC_start_single_conversion();
			
			// wait for the ADC to finish
			while(hr_samples[sample[0]] == 0)
			{
				//do nothing
			}
			
			hr_sum = 0;
			hr_min_val = 255;
			hr_max_val = 0;			
			for (i=0; i<t_hr_samples; i++)
			{
				if (hr_min_val > hr_samples[i])
				{
					hr_min_val = hr_samples[i];
				}
				if (hr_max_val < hr_samples[i])
				{
					hr_max_val = hr_samples[i];
				}
				hr_sum = hr_sum + hr_samples[i];
			}
			avg_hr_val = hr_sum / t_hr_samples;

			hr_rlng_avg = (hr_samples[sample[0]] + hr_samples[sample[1]] + hr_samples[sample[2]])/3;

			if ((hr_rlng_avg > avg_hr_val) && (beat_started == 0))
			{
				beat_started = 1;
				btwn_beats[beat] = btwn_bt_ctr;
				btwn_bt_ctr = 0;
				SET_BIT(PORTC, 5);
				
			}
			else if ((hr_rlng_avg < avg_hr_val) && (beat_started == 1))
			{
				beat_started = 0;
				CLEAR_BIT(PORTC,5);
			}

			time_sum = 0;
			for (i=0; i< beats_to_avg; i++)
			{
				time_sum = time_sum+btwn_beats[i];
			}
			avg_beat_time = time_sum / beats_to_avg;
			avg_hr = 60000 / avg_beat_time;
			
			if ((hr_max_val-hr_min_val)<5)
			{
			//	lcd_puts("woops");
			}

			for (i=0; i<3; i++)
			{
				sample[i]++;
				if (sample[i] >= t_hr_samples)
				{
					sample[i] = 0;
				}	
			}
			beat++;
			if (beat >=beats_to_avg)
			{
				beat = 0;
			}

		}	

		//go back to sleep now
		sleep_now();	// sleep for 1 ms
	}
}






















