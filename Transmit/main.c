//RF transmission
#define F_CPU 8000000UL /* 8 MHz Internal Oscillator */
#include <avr/io.h>
#include <util/delay.h>
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

int main(void){
	//initialize ports
	SET_BIT(DDRD,1);
	CLEAR_BIT(PORTD, 0);	

	// Initialize AVR for use with nRF24L01
	nRF24L01_init();
	// Wait for nRF24L01 to come up
	_delay_ms(50);
	// Activate interrupts
	sei();
	// Configure nRF24L01
	nRF24L01_config();
	// Test communication
	while(1)
	{
		//toggle between value of 0x01 and 0x02
		if (nRF24L01_data[0] == 0x01)
		{
			nRF24L01_data[0] = 0x02;
		}
		else
		{
			nRF24L01_data[0] = 0x01;
		}

		//send the data to the nRF24L01+
		nRF24L01_send(buffer,1);
		_delay_ms(1);	
		sleep_ms(1000);	//wait 1 second before sending new data
	}
}























