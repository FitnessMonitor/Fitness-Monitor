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
	while(!(UCSR0A & (1<<RXC0))){} //wait for data
	return UDR0;
}

void USARTWriteChar(char data)
{	
	while(!(UCSR0A & (1<<UDRE0))){};//wait until transmitter is ready
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


