//RF transmission
#define F_CPU 8000000UL /* 8 MHz Internal Oscillator */
#include <avr/io.h>
#include <util/delay.h>
//#include "../spi.c"
//#include "../nRF24L01.c"
#include "../sleep.c"
#include "../ADC.c"

volatile uint8_t heart_rate_data[40];
volatile uint8_t current_sample;
uint16_t hr_sum;
uint16_t hr_avg;
volatile int8_t ADC_running;
volatile uint8_t ms_count;
uint8_t i; //generic loop counter
uint8_t beat_state;

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
	TOGGLE_BIT(PORTD,0);
	if (ms_count >= 100)
	{
		ms_count = 0;
		ADC_enable();
		ADC_interrupt_enable();
		ADC_running = 1;
		ADC_start_conversion();
		current_sample++;
		if (current_sample >=40)
		{
			current_sample = 0;	
		}
	}	
}

ISR(ADC_vect)
{
	ADC_running = 0;
	heart_rate_data[current_sample] = ADCH;
}

int main(void){
	//initialize ports
	timer2_1ms_setup();
	update_time();
	SET_BIT(DDRD,1);
	SET_BIT(DDRD,0);
	CLEAR_BIT(PORTD, 0);	

	ADC_init();	//initialize ADC
	
	
	ms_count = 0;
	while(1)
	{
		while (ADC_running == 1)
		{
		}

		hr_sum = 0;		//clear the sum
		for (i=0; i<40; i++)
		{
			hr_sum = hr_sum + heart_rate_data[i];
		}
		hr_avg = hr_sum / 40;
		if (heart_rate_data[current_sample] >= (hr_avg+1))
		{
			SET_BIT(PORTD, 1);
		}
		else 
		{
			CLEAR_BIT(PORTD,1);
		}
		sleep_now();	// sleep for 1 ms
	}
}























