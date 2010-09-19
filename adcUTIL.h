#define in1 0x4C	//1.5V, walking hard
#define in2 0x47	//1.4V, walking softly
#define in3 0x44	//1.34V, walking very softly


int initADC(void)
{
//left adjust (use only ADCH),select ADC0 (PC0) as input
ADMUX |= (1<<ADLAR);     
//turn on ACD, start conversion, auto trigger
//(free running mode), set prescaler to 8 (ck=500Khz)
ADCSRA |= (1<<ADEN)|(1<<ADSC)|(1<<ADATE)|(1<<ADPS2)|(1<<ADPS1);
//result of conversion stored in ADC9-ADC0
//single ended conversion: ADC = (Vin*1024)/Vref 
return 1;
}


