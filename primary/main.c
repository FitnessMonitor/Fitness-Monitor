#define F_CPU 8000000UL /* 8 MHz Internal Oscillator */
#include <avr/io.h>
#include <util/delay.h>
//#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/power.h>
//#include <stdlib.h>
//#include "../lib/nRF24L01.c"
//#include "../lib/spi.c"
#include "../lib/sleep.c"
#include "../lib/ADC.c"
#include "../lib/ff.c"
#include "../lib/diskio.c"
#include "../lib/lcd.c"
#include "../lib/lcdfont.c"


//Macros for setting, clearing and toogleing bits.
#define SET_BIT(PORT, BITNUM) ((PORT) |= (1<<(BITNUM)))
#define CLEAR_BIT(PORT, BITNUM) ((PORT) &= ~(1<<(BITNUM)))
#define TOGGLE_BIT(PORT, BITNUM) ((PORT) ^= (1<<(BITNUM)))

volatile uint16_t ms_counter = 0;
uint8_t disp_buffer[512];

ISR(TIMER0_COMPA_vect) {  
//  disk_timerproc();
}

ISR(TIMER2_OVF_vect)	//when timer 2 interrupts
{			//wake up from sleeping
	sleep_disable();
	timer2_1ms_reset();	//reset timer to interrupt in 1ms
	ms_counter++;
}

ISR(ADC_vect)
{
//hr_samples[sample[0]] = ADCH;
}

ISR( PCINT2_vect ) 
{
//	nRF24L01_interrupt ();
}


static void IoInit ()
{
	PORTB = 0b10110000; 	// Port B
	DDRB  = 0b00001000;

	DDRC |= 1<<PC3;
	DDRC |= 1<<PC2;

	TIMSK0 |= 1 << OCIE0A;  //enable interrupt for timer match a 
	OCR0A = 78;  // 10 ms interrupt at 8MHz
	TCCR0B |= (1 << CS02) | (1 << CS00);  //speed = F_CPU/1024
	power_timer0_enable();

	sei();

}


void setup(void) {
	// turn on backlight
	BLA_DDR |= _BV(BLA);
	BLA_PORT |= _BV(BLA);

	LED_DDR |= _BV(LED);
	//IoInit();
	
	st7565_init();
	st7565_command(CMD_DISPLAY_ON);
	st7565_command(CMD_SET_ALLPTS_NORMAL);
	clear_screen();
	clear_buffer(disp_buffer);

	//testdrawchar(disp_buffer);
	drawstring(disp_buffer, 0, 3, "Fitness Monitor");
	write_buffer(disp_buffer);
	clear_buffer(disp_buffer);
}

int main(void){
	
	setup();
	uint8_t line = 0;

	//initialize timer 2 to interrupt ever 1ms
	timer2_1ms_setup();
	while(1)
	{	
		if (ms_counter % 50)	//sample every 50ms		
		{
			//sample the ADC and store result somewhere	
		}
		if (ms_counter == 10000) //every 10 seconds
		{
				
			ms_counter = 0; //reset counter
			line ++;
			if (line == 4) line = 0;
			clear_buffer(disp_buffer);
			drawstring(disp_buffer, 0, line, "test");
			write_buffer(disp_buffer);
			FATFS FileSystemObject;



			DSTATUS driveStatus = disk_initialize(0);
	

			if ((driveStatus & STA_NODISK) || (driveStatus & STA_NOINIT)){
		   		drawstring(disp_buffer, 0, 0, "ERROR");
		   		write_buffer(disp_buffer);	
			}
			else{

				if(f_mount(0, &FileSystemObject)!=FR_OK) {
					//PORTC |= (1<<PC2);
			    		drawstring(disp_buffer, 0, 0, "ERROR");
			   		write_buffer(disp_buffer);

				}
				else{
					drawstring(disp_buffer, 0, 1, "Mounted");
			   		write_buffer(disp_buffer);

				}
				FIL logFile;
				if(f_open(&logFile, "/20101117.txt", FA_READ | FA_WRITE | FA_OPEN_ALWAYS)!=FR_OK) {
					drawstring(disp_buffer, 0, 2, "File not created");

			   		write_buffer(disp_buffer);
				}
				else{
					drawstring(disp_buffer, 0, 2, "File created");

			   		write_buffer(disp_buffer);
					unsigned int bytesWritten;
					f_write(&logFile, "123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n123456789\n", 100, &bytesWritten);
					f_write(&logFile, "987654321\n987654321\n987654321\n987654321\n987654321\n987654321\n987654321\n987654321\n987654321\n987654321\n", 100, &bytesWritten);

					drawstring(disp_buffer, 0, 3, "File Written");
			   		write_buffer(disp_buffer);
					//Close and unmount. 
					f_close(&logFile);

				}
			}
		
		}

		sleep_now();	// sleep until timer2 interrupt
	}
}














