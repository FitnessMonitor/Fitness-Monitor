#include "primary.h"


static void IoInit ()
{
	PORTB = 0b10110000; 	// Port B
	DDRB  = 0b00001000;

	DDRC |= 1<<PC3;
	DDRC |= 1<<PC2;

	sei();
}

void setup(void) {
	// turn on backlight
	BLA_DDR |= _BV(BLA);
	BLA_PORT |= _BV(BLA);

	LED_DDR |= _BV(LED);
	IoInit();
	
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

void disp_hms(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
	char display_time[10];
	sprintf( &display_time[0], "%d:%d:%d", (int) hours, (int) minutes, (int) seconds );
	drawstring( disp_buffer, 0, 0, &display_time[0] );
	write_buffer(disp_buffer);
}


void i2s(int i,char *s) // Convert Integer to String
{
	char sign;
	short len;
	char *p;
	sign = '+';
	len = 0;
	p = s;
	if(i < 0){
		sign='-';
		i=-i;
	}
	do {
		*s=(i%10)+'0';
		s++;
		len++;
		i /= 10;
	} while (i != 0);
	if(sign == '-'){
		*s = '-';
		s++;
		len++;
	}
	for(i = 0; i < len/2; i++){
		p[len] = p[i];
		p[i] = p[len-1-i];
		p[len-1-i] = p[len];
	}
	p[len] = 0;
}


//SD card functions
int init_sdcard(void)
{
	DSTATUS driveStatus = disk_initialize(0);
	if ((driveStatus & STA_NODISK) || (driveStatus & STA_NOINIT)) //check for initialization errors
	{
   		drawstring(disp_buffer, 0, 1, "SD Initialization Error");
   		write_buffer(disp_buffer);
		return 1;	//error 1 initialization failed
	}
	else{	//dont try mounting if initialization failed
		if(f_mount(0, &FileSystemObject)!=FR_OK) {
			//flag error
			drawstring( disp_buffer, 0, 1, "File System Mounting Error" );
			write_buffer(disp_buffer);
			return 2;	//error 2 mounting failed
		}
	}
	return 0; //no errors
}

int sdcard_open(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
	//unsigned int bytesWritten;
	char file_name[13];
	char *file_name_text = &file_name[0];
	sprintf(file_name_text, "/%d_%d_%d.txt", (int) hours, (int) minutes, (int) seconds );
	if(f_open(&logFile, file_name_text, FA_READ | FA_WRITE | FA_OPEN_ALWAYS)!=FR_OK) {
		//flag error
		drawstring( disp_buffer, 0, 1, "f_open Error" );
		write_buffer(disp_buffer);
		return 1;
	}
	return 0;
}
void sdcard_close()
{
	f_close(&logFile);
}



// accelerometer functions


void get_steps(uint8_t *points, uint8_t size, uint8_t * avg, uint8_t * steps, uint8_t * activity_level)
{
	int i;
	uint16_t sum = 0;
	*steps = 0;
	for ( i = 0 ; i < size ; i++ )
	{
		sum += points[i];	
	}
	//(*avg) = sum / size;
	
	uint8_t max = (*avg)+5;
	uint8_t min = (*avg)-5;
	for ( i = 1 ; i < size ; i++ )
	{
		//count steps
		if ((points[i] <= min) && (points[i] >= max))
		{
			//(*steps)++;
		}
		//figure out activity level (average varience)
		if (points[i] >= *avg)
		{
			sum += (points[i] - (*avg));
		}
		else
		{
			sum += ((*avg) - points[i]);
		}
	}
	//(*activity_level) = sum / size;	
}


