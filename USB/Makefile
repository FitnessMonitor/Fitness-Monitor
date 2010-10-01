CC=avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=atmega168
OBJ2HEX=avr-objcopy 
UISP=uisp 
ASRC=xitoa.S
TARGET=main

program : $(TARGET).hex
	avrdude -c usbtiny -p m168 -U flash:w:main.hex -U lfuse:w:0x62:m -U hfuse:w:0xdf:m -U efuse:w:0x01:m 
%.obj : %.o
	$(CC) $(CFLAGS) $< -o $@

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

clean :
	rm -f *.hex *.obj *.o

