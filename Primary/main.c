#define F_CPU 8000000UL /* 8 MHz Internal Oscillator */
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "../spi.c"
#include "../nRF24L01.c"
#include "../sleep.c"
//#include "../ADC.c"
//#include "../lcd.c"
//#include "../rtc.c"


//Macros for setting, clearing and toogleing bits.
#define SET_BIT(PORT, BITNUM) ((PORT) |= (1<<(BITNUM)))
#define CLEAR_BIT(PORT, BITNUM) ((PORT) &= ~(1<<(BITNUM)))
#define TOGGLE_BIT(PORT, BITNUM) ((PORT) ^= (1<<(BITNUM)))

volatile uint32_t RTC_sec_counter;
volatile uint16_t RTC_ms_counter;

uint8_t nRF24L01_data[32];
uint8_t *buffer =  &nRF24L01_data[0];
uint8_t buffersize = sizeof(nRF24L01_data);

ISR( PCINT2_vect ) 
{
	nRF24L01_interrupt ();
}

ISR(TIMER2_OVF_vect)	//when timer 2 interrupts
{			//wake up from sleeping
	sleep_disable();
	timer2_1ms_reset();	//reset timer to interrupt in 1ms	
	RTC_ms_counter++;
}

ISR(ADC_vect)
{
	//hr_samples[sample[0]] = ADCH;
}

int main(void){

	//initialize ports
	SET_BIT(DDRC,5);

	// Initialize AVR for use with nRF24L01
	nRF24L01_init();

	// Wait for nRF24L01 to come up
	_delay_ms(50);

	// Activate interrupts
	sei();

	// Configure nRF24L01
	nRF24L01_config();

	//set up PORTD for indicating LED lights
	SET_BIT(DDRD, 0);
	SET_BIT(DDRD, 1);	
	
	int8_t Rx_active = 0;
	int16_t Rx_count = 0;


	while(1)
	{
		
		if(RTC_ms_counter >= 1000)
		{
			RTC_sec_counter++;
			RTC_ms_counter = 0;
			Rx_active = 1;
			nRF24L01_start_RX ();
		}
		if((Rx_active))
		{
			//check if data has been recieved
			if (nRF24L01_data_ready())
			{
				//store the packet in a buffer
				nRF24L01_get_data(buffer);
				Rx_active = 0;
				nRF24L01_powerdown ();
			}
		}

		// turn on an appropriate led based uppon the message recieved
		if (nRF24L01_data[0] == 0x01)		//LED 1
		{
			SET_BIT(PORTD, 0);		//turn on led 1
			CLEAR_BIT(PORTD,1);		//turn off led 2
		}
		else if (nRF24L01_data[0] == 0x02)	//LED2
		{
			SET_BIT(PORTD, 1);		//turn on led 2
			CLEAR_BIT(PORTD,0);		//turn off led 1
		}
	

		//go back to sleep now
		sleep_now();	// sleep for 1 ms
	}
}



















