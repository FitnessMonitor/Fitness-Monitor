/*------------------------------------------------*/
/* UART functions                                 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart.h"

#define   BAUD      9600

void USART_Transmit( unsigned char txData )
{
   /* Wait for empty transmit buffer */
   while ( !( UCSR0A & (1<<UDRE0)) );
   /* Put data into buffer, sends the data */
   UDR0 = txData;
}

void USART_set_baud_rate(double baudrate)
{
   // calculate division factor for requested baud rate, and set it
   int bauddiv = ((F_CPU+(baudrate*8L))/(baudrate*16L)-1);
   UBRR0L= bauddiv;
#ifdef UBRR0H
   UBRR0H= (bauddiv>>8);
#endif
}

/* Initialize UART */

void uart_init()
{
   UCSR0B = (1<<RXEN0)|(1<<TXEN0);      // Turn on U(S)ART port
   UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);   // Set frame format: 8 data bits, 1 stop bit, no parity
   USART_set_baud_rate(BAUD); //Set baud rate
}


/* Get a received character */
uint8_t uart_get ()
{
   unsigned char d;
   while ((UCSR0A & (1 << RXC0)) == 0) {}; // Do nothing until data have been recieved and is ready to be read from UDR
   d=UDR0;
   return d;
}

/* Transmit a character */
void uart_put(uint8_t d)
{
   
   USART_Transmit( d );
}

/* Transmit a string */
void uart_puts(const char *s)
{
   while (*s)
      USART_Transmit( *s++ );
} 
