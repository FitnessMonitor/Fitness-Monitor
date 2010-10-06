#define LED_ON 	PORTC |= (1<<PORTC1);  //turn on blue LED
#define LED_OFF PORTC &= ~(1<<PORTC1); //turn off blue LED

void pulseLED(void)	
{
	DDRC |= 0x02;		//set PORTC1 for output
	LED_ON;
	_delay_ms(200);
	LED_OFF;
}

void ERROR(void)
{	
	DDRC |= 0x02;		//set PORTC1 for output
	LED_ON;
	_delay_ms(200);
	LED_OFF;
	_delay_ms(200);
	LED_ON;
	_delay_ms(200);
	LED_OFF;
	_delay_ms(200);
	LED_ON;
	_delay_ms(200);
	LED_OFF;
	_delay_ms(200);
	LED_ON;
	_delay_ms(200);
	LED_OFF;
	_delay_ms(200);
}
