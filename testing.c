#include <avr/io.h>
#include <util/delay.h>
#include "uartUTIL.h"
#include "adcUTIL.h"

char *ptr1 = " hard step ";
char *ptr2 = " soft step ";
char *ptr3 = " very soft step ";

int main(void)
{
	int stepCNT=0;	//count steps taken
	USARTInit(25);	//UART setup
	initADC();	// ADC setup 
	
	

	while(1)
	{
		
		if(ADCH>in1)	//hard step
		{
			stepCNT++;
			uart_puts(ptr1);
					
		}

	

		else if(ADCH>in2)	//soft step
		{
			stepCNT++;
			uart_puts(ptr2);
					
		}

		else if(ADCH>in3)	//very soft step
		{
			stepCNT++;
			uart_puts(ptr3);
					
		}

		_delay_ms(50);	//wait a bit 

	}


return 0;   
}







