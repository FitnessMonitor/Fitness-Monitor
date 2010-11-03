#define F_CPU 1000000UL /* 8 MHz Internal Oscillator */
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
//#include "../spi.c"
//#include "../nRF24L01.c"
//#include "../sleep.c"
//#include "../ADC.c"
//#include "../lcd.c"
//#include "../rtc.c"
#include "../uart.c"

//Macros for setting, clearing and toogleing bits.
#define SET_BIT(PORT, BITNUM) ((PORT) |= (1<<(BITNUM)))
#define CLEAR_BIT(PORT, BITNUM) ((PORT) &= ~(1<<(BITNUM)))
#define TOGGLE_BIT(PORT, BITNUM) ((PORT) ^= (1<<(BITNUM)))

int main(void){

	//setup UART
	USARTInit(MYUBRR);
	
	char *ptr3 = " Working \r\n ";
	while(1)
	{	
		uart_puts(ptr3);
		_delay_ms(1000);	
	}

}






















