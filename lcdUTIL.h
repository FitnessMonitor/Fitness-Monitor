#define CS		PORTC2 
#define A0		PORTC3
#define SDA		PORTC4
#define SCL		PORTC5
//#define P1		PORTC


void init_LCD() //controller initialization
{
	DDRC |= 0x3C; //set pins C2-C5 for output

/*
	DDRC = 0x7C;	//set PORTC output pins for TWI

	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);	//send start condition
	while (!(TWCR & (1<<TWINT)));	//wait for TWINT flag set
	if ((TWSR & 0xF8) != START)		//check TWI status register
	ERROR();
	TWDR = SLA_W;	//load SLA_W into TWDR
	TWCR = (1<<TWINT) | (1<<TWEN);	//clear TWINT bit to start transmission
	while(!(TWCR & (1<<TWINT)));	//wait for TWINT flag set
	if ((TWSR & 0xF8) != MT_SLA_ACK);	//check value of TWI status register
	ERROR();
	TWDR = DATA;	//load data into TWDR
	TWCR = (1<<TWINT)|(1<<TWEN);	//clear TWINT bit to start transmission
	while (!(TWCR & (1<<TWINT)));	//wait for TWINT flag
	if ((TWSR & 0xF8) != MT_DATA_ACK)	//check TWI status register value
	ERROR();
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);	//transmit stop condition
*/
	comm_out(0xA0);
	comm_out(0xAE);
	comm_out(0xC0);
	comm_out(0xA2);
	comm_out(0x2F);
	comm_out(0x26);
	comm_out(0x81);
	comm_out(0x2F);
}

void data_out(unsigned char i) //Data Output Serial Interface
{
	unsigned int n;
	PORTC &= ~(1<<CS);			//CS = 0;
	PORTC |= (1<<A0);			//A0 = 1;

	for(n=0; n<8; n++)
	{
		PORTC &= ~(1<<SCL);		//SCL = 0;
		i = SDA;
		_delay_ms(2);			//delay(2);
		PORTC |= (1<<SCL);		//SCL = 1;
		i <<=1;					//shift left
	}
	PORTC |= (1<<CS);			//CS = 1;
}
void comm_out(unsigned char j) //Command Output Serial Interface
{
	unsigned int n;
	PORTC &= ~(1<<CS);			//CS = 0;
	PORTC &= ~(1<<A0);			//A0  = 0;
	for(n=0; n<8; n++)
	{
		j <<=1;					//shift left
		PORTC &= ~(1<<SCL);		//SCL = 0;
		j = SDA;
		_delay_ms(2);			//delay(2);
		PORTC |= (1<<SCL);		//SCL = 1;	
	}
	PORTC |= (1<<CS);			//CS = 1;
}

