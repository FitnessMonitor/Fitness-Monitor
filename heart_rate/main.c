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

volatile uint8_t hr_samples[40];	//ADC sample values	
volatile uint16_t hr_interval[10];	//time between beats
volatile uint8_t interval;	//location in array of interval times
volatile uint8_t sample[3];	//location in array of hr samples
uint16_t hr_sum;
uint16_t hr_rlng_avg;
uint16_t hr_avg;
uint32_t interval_sum;
uint16_t avg_interval;
float hr_period;
float hr_frequency;
volatile uint8_t beat_high;
char frequency[2];

volatile int8_t ADC_running;
volatile uint8_t ms_count;
uint8_t i; //generic loop counter
volatile uint16_t btwn_beat_ctr;	//stores time between beats

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
	ms_count++;
	btwn_beat_ctr++;	
}

ISR(ADC_vect)
{
	ADC_running = 0;
	hr_samples[sample[0]] = ADCH;
}

int main(void){
	//initialize ports
	timer2_1ms_setup();
	update_time();
	SET_BIT(DDRD,1);
	SET_BIT(DDRD,0);
	CLEAR_BIT(PORTD, 0);	

	ADC_init();	//initialize ADC
	
	sample[0] = 2;	// these are the starting locations in the array of hr_samples
	sample[1] = 1;
	sample[2] = 0;
	
	ms_count = 0;
	lcd_init(LCD_DISP_ON);

	lcd_clrscr();

	lcd_puts("snow rules");
	while(1)
	{
		if (ms_count >= 100)
		{
			lcd_clrscr();
			//clear the counter
			ms_count = 0;
	
			//start an ADC
			ADC_enable();	
			ADC_interrupt_enable();
			ADC_running = 1;
			ADC_start_conversion();
			

			//update locations
			for (i = 0; i<3; i++)
			{
				sample[i]++;
				if (sample[i] >=40)
				{
					sample[i] = 0;	
				}
			}
			if (interval >= 10)
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
				//SET_BIT(PORTD, 1);
				hr_interval[interval] = btwn_beat_ctr;	//store the time between beats
				btwn_beat_ctr = 0; //reset the counter
			}
			else if (( hr_rlng_avg < hr_avg ) && (beat_high == 1))
			{
				beat_high = 0;
				//CLEAR_BIT(PORTD,1);
			}

			//calculate average heart rate
			for (i = 0; i<10; i++)
			{
				interval_sum = interval_sum + hr_interval[i];
			}
			hr_frequency = (600000/interval_sum);
			
			ultoa((uint16_t)hr_frequency, frequency, 10 );
			lcd_clrscr();
			lcd_puts("heart_rate:\n");
			for (i = 0; i<2; i++)
			{
				lcd_putc(frequency[i]);
			}

		}

		//go back to sleep now
		sleep_now();	// sleep for 1 ms
	}
}






















