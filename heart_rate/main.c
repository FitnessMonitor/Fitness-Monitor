#define F_CPU 1000000UL /* 1 MHz Internal Oscillator */
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "../nRF24L01.c"
#include "../spi.c"
#include "../sleep.c"
#include "../rtc.c"
#include "../uart.c"
#include "../ADC.c"

//Macros for setting, clearing and toogleing bits.
#define SET_BIT(PORT, BITNUM) ((PORT) |= (1<<(BITNUM)))
#define CLEAR_BIT(PORT, BITNUM) ((PORT) &= ~(1<<(BITNUM)))
#define TOGGLE_BIT(PORT, BITNUM) ((PORT) ^= (1<<(BITNUM)))

volatile uint16_t ms_counter = 0;

ISR(TIMER2_OVF_vect)	//when timer 2 interrupts
{			//wake up from sleeping
	sleep_disable();
	timer2_1ms_reset();	//reset timer to interrupt in 1ms
	ms_counter++;
}

ISR(ADC_vect)
{
	//hr_samples[sample[0]] = ADCH;
}


ISR( PCINT2_vect ) 
{
	nRF24L01_interrupt ();
}


int main(void){
	
	//initialize ports
	DDRC |= 1<<PC5;

	// Initialize AVR for use with nRF24L01
	nRF24L01_init();
	// Wait for nRF24L01 to come up
	_delay_ms(50);

	//initialize timer 2 to interrupt ever 1ms
	timer2_1ms_setup();

	// Configure nRF24L01
	nRF24L01_config();

	//setup UART
	USARTInit(MYUBRR);

	//initialize ADC
	ADC_init(ADC0);
	char charstring[] = "abcdefg   "; 
	char *ptr = &charstring[0];
	uint16_t hr_sample[150];
	uint8_t hr_index = 0;
	uint8_t bpm[10] = {50, 50, 50, 50, 50, 50, 50, 50, 50, 0};
	uint8_t bpm_index = 0;
	uint8_t bpm_avg;
	uint8_t i;
	nRF24L01_data[0] = 0x02;
	while(1)
	{	
		if ((ms_counter % 50)-10 == 0)	//sample the Heart Rate signal every 50ms
		{
			ADC_start_single_conversion();
			
			//wait for the ADC to finish
			while((ADCSRA & (1<<ADSC))){};

			hr_sample[hr_index] = ADCH; 
			hr_index++;

			//if (hr_sample[hr_index]> 80) SET_BIT(PORTC,5);
			//else CLEAR_BIT(PORTC,5);
		}

		
		if (ms_counter == 9950)	//calculate beats after last sample in 10sec interval
		{
			uint8_t beat_started = 0;
			uint8_t beat_count = 0;
			uint8_t interval = 0;
			//calculate how many beats occured
			for (i = 0; i< hr_index; i++)
			{
				interval++;
				if ((hr_sample[i] >= 80) && !beat_started && (interval > 1))
				{
					beat_count++;
					interval = 0;
					beat_started = 1;
				}
				else if ((hr_sample[i] < 80) && beat_started && (interval > 1))
				{
					interval = 0;
					beat_started = 0;
				}
			}
			hr_index = 0;
			
			bpm[bpm_index] = beat_count*10;
			if (bpm_index < 5) bpm_index++;
			else bpm_index = 0;
					
			bpm_avg = 0;
			for (i = 0; i<6; i++)
			{
				bpm_avg = bpm_avg + bpm[bpm_index];
			}
			bpm_avg = bpm_avg / 6;						
		
		}		
		
		if (ms_counter > 1000)		//on every 1000ms (1sec)
		{
			ms_counter = 0;		//reset counter
			uart_puts(ptr); 
			if (nRF24L01_data[0] == 0x01)
			{
				nRF24L01_data[0] = 0x02;
			}	
			else
			{
				nRF24L01_data[0] = 0x01;
			}
			nRF24L01_send(buffer,1);	
		}
		sleep_now();	// sleep until timer2 interrupt
	}

}















