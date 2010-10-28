#ifndef _USART_H_
#define _USART_H_

#define FOSC 1000000	//clock speed
#define BAUD 4800
#define MYUBRR FOSC/16/BAUD-1

void USARTInit(int ubrr_value);
char USARTReadChar(void);
void USARTHex(uint8_t data);
void USARTWriteChar(char data);
void uart_puts(char *s);

#endif //_USART_H_

// eof

