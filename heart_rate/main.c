#define F_CPU 1000000UL /* 8 MHz Internal Oscillator */
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
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


int main(void){
	
	//initialize ports
	DDRC |= 1<<PC5;

	//initialize timer 2 to interrupt ever 1ms
	timer2_1ms_setup();

	//setup UART
	USARTInit(MYUBRR);

	//initialize ADC
	ADC_init(ADC0);
	
	char hr_spl_srting[5];
	char *hr_ptr = &hr_spl_srting;
	char *newline = "\r\n";
	char *space = " ";
	char *comma = ",";
	uint16_t hr_sample[1500];
	uint16_t num = 0;
	uint16_t m;
	while(1)
	{	
		if (hr_counter >100)
		{
			hr_counter = 0;
			ADC_start_single_conversion();
			
			//wait for the ADC to finish
			while((ADCSRA & (1<<ADSC))){};

			hr_sample[num] = ADCH; 
			if (hr_sample[num]> 80) SET_BIT(PORTC,5);
			else CLEAR_BIT(PORTC,5);
			num++;
		}
		else if (ms_counter > 10000)		//on every 1000ms (1sec)
		{
			ms_counter = 0;		//reset counter
			
			for (m = 0; m<num; m++)
			{
				ultoa (hr_sample[m], hr_spl_srting ,10);
				uart_puts(hr_ptr);	//send string 
				uart_puts(comma);
			}	
			uart_puts(newline);
			
			if (num == 3000) num = 0;
		}
		sleep_now();	// sleep until timer2 interrupt
	}

}















