#define F_CPU 1000000UL /* 8 MHz Internal Oscillator */
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "../sleep.c"
#include "../rtc.c"
#include "../uart.c"

//Macros for setting, clearing and toogleing bits.
#define SET_BIT(PORT, BITNUM) ((PORT) |= (1<<(BITNUM)))
#define CLEAR_BIT(PORT, BITNUM) ((PORT) &= ~(1<<(BITNUM)))
#define TOGGLE_BIT(PORT, BITNUM) ((PORT) ^= (1<<(BITNUM)))

volatile uint16_t ms_counter;


ISR(TIMER2_OVF_vect)	//when timer 2 interrupts
{			//wake up from sleeping
	sleep_disable();
	timer2_1ms_reset();	//reset timer to interrupt in 1ms
	ms_counter++;
}


int main(void){
	
	//initialize ports
	DDRC |= 1<<PC5;

	//initialize timer 2 to interrupt ever 1ms
	timer2_1ms_setup();

	//setup UART
	USARTInit(MYUBRR);
	
	char *ptr3 = "  Working \r\n  ";

	while(1)
	{	
		if (ms_counter > 1000)		//on every 1000ms (1sec)
		{
			TOGGLE_BIT (PORTC,5);
			ms_counter = 0;		//reset counter
			uart_puts(ptr3);	//send string 
		}
		sleep_now();	// sleep until timer2 interrupt
	}

}















