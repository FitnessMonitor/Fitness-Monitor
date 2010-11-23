#include "uart.h"

void USARTInit(int ubrr_value)
{
//ubrr=(f_osc/(16*Baud Rate))-1
//for f_osc = 1Mhz, Baud Rate = 2400, ubrr = 25
UBRR0H = (ubrr_value>>8);	//shift 8 bits?
UBRR0L = ubrr_value;  //set baud rate
// asynchronous mode, no parity, 1 stop bit, 8 bit size
UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
//receive enable, tansmit enable
UCSR0B = (1<<RXEN0)|(1<<TXEN0);
}

char USARTReadChar()
{	
<<<<<<< HEAD
//	wait until data is available
//	_delay_ms(1000);		//wait 1 seconds
	while(!(UCSR0A & (1<<RXC0))) //while 1, loop
	{	
		//do nothing
	}

=======
	while(!(UCSR0A & (1<<RXC0))){} //wait for data
>>>>>>> 5793541bd196650923ae10c25b167ddf9128cc26
	return UDR0;
}

void USARTWriteChar(char data)
{	
<<<<<<< HEAD
	while(!(UCSR0A & (1<<UDRE0)))	//wait until transmitter is ready
	{
		//do nothing
	}
=======
	while(!(UCSR0A & (1<<UDRE0))){};//wait until transmitter is ready
>>>>>>> 5793541bd196650923ae10c25b167ddf9128cc26
	UDR0=data;  //write data to USART buffer
}

void uart_puts(char *s) 
{	
	while(*s) //  loop until *s != NULL
	{
		USARTWriteChar(*s);
		s++;
	}
}


