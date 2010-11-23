CC=avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=atmega328p
OBJ2HEX=avr-objcopy 
UISP=uisp 
TARGET=main

program : $(TARGET).hex
#Normal
	avrdude -c usbtiny -p m328p -U flash:w:main.hex
#switch to 8MHZ
#	avrdude -c usbtiny -p m328p -U flash:w:main.hex -U lfuse:w:0xe2:m  -U hfuse:w:0xd9:m -U efuse:w:0x07:m 
#switch to 1MHZ
#	avrdude -c usbtiny -p m328p -U flash:w:main.hex -U lfuse:w:0x62:m  -U hfuse:w:0xd9:m -U efuse:w:0x07:m

%.obj : %.o
	$(CC) $(CFLAGS) $< -o $@

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

clean :
	rm -f *.hex *.obj *.o

