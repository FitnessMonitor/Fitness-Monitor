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
volatile uint8_t hr_counter = 20;
volatile uint8_t transmit_counter = 0;

ISR(TIMER2_OVF_vect)	//when timer 2 interrupts
{			//wake up from sleeping
	sleep_disable();
	timer2_1ms_reset();	//reset timer to interrupt in 1ms
	ms_counter++;
	hr_counter++;
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
	
	char hr_spl_srting[5];
	char *hr_ptr = &hr_spl_srting[0];
	char *newline = "\r\n";
	//char *space = " ";
	char *comma = ",";
	uint16_t hr_sample[1500];
	uint16_t num = 0;
	uint16_t m;
	nRF24L01_data[0] = 0x02;
	while(1)
	{	
		if (0)
		{
			hr_counter = 0;
			ADC_start_single_conversion();
			
			//wait for the ADC to finish
			while((ADCSRA & (1<<ADSC))){};

			hr_sample[num] = ADCH; 
			if (hr_sample[num]> 80) SET_BIT(PORTC,5);
			else CLEAR_BIT(PORTC,5);
			//num++;

		}
		else if (ms_counter > 1000)		//on every 1000ms (1sec)
		{
			ms_counter = 0;		//reset counter

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















