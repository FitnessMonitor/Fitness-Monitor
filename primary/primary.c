#include "primary.h"


void i2s(int i,char *s)	// Convert Integer to String
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


void test_write()
{
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
