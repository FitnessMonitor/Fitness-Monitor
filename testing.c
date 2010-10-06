#include <avr/io.h>
#include <util/delay.h>
#include "adcUTIL.h"
#include "ledUTIL.h"
#include "lcdUTIL.h"

char *ptr1 = " hard step ";
char *ptr2 = " soft step ";
char *ptr3 = " very soft step ";
char i;

int main(void)
{
	
	int stepCNT=0;	//count steps taken
	initADC();		//ADC setup 
	init_LCD();		//LCD setup

	_delay_ms(10);

	data_out(*ptr1);	//output text to LCD
 			
	while(1)
	{
		
		
		if(ADCH>in1)		//very soft step
		{
			stepCNT++;
			pulseLED();
					
		}


		else if(ADCH>in2)	//soft step
		{
			stepCNT++;
			pulseLED();
			pulseLED();
				
		}

		else if(ADCH>in3)	//hard step
		{
			stepCNT++;
			pulseLED();
			pulseLED();
			pulseLED();
					
		}

		_delay_ms(10);	//wait a bit 

	}

return 0;   
}







