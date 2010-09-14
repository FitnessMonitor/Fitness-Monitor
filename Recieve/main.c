//RF Recieve
#define F_CPU 8000000UL /* 8 MHz Internal Oscillator */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "../spi.c"
#include "../nRF24L01.c"
#include "../sleep.c"

uint8_t nRF24L01_data[32];
uint8_t *buffer =  &nRF24L01_data[0];
uint8_t buffersize = sizeof(nRF24L01_data);

//Macros for setting, clearing and toogleing bits.
#define SET_BIT(PORT, BITNUM) ((PORT) |= (1<<(BITNUM)))
#define CLEAR_BIT(PORT, BITNUM) ((PORT) &= ~(1<<(BITNUM)))
#define TOGGLE_BIT(PORT, BITNUM) ((PORT) ^= (1<<(BITNUM)))

#define __AVR_ATmega168__

ISR( PCINT2_vect ) 
{
	nRF24L01_interrupt ();
}

ISR(TIMER2_OVF_vect)	//when timer 2 interrupts
{			//wake up from sleeping
	sleep_disable();
}

ISR( BADISR_vect ){} //handle potential bad interrupt vectors by doing nothing


int main(void){
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

	//wait 300ms before starting 
	_delay_ms(3000);

	while(1)	//infinite loop
	{
		while (!nRF24L01_data_ready())		//wait for packet from nRF24L01+
		{
		sleep_ms(1);
		}
		_delay_us(100);				//wait 100us before reading the data
		nRF24L01_get_data(buffer);			//get the data from the nRF24L01+ and store it in the buffer


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
	}
}
//test




















