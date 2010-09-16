CC=avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=atmega328p
OBJ2HEX=avr-objcopy 
UISP=uisp 
ASRC=xitoa.S
TARGET=main

program : $(TARGET).hex
	avrdude -c usbtiny -p m328p -U flash:w:main.hex
%.obj : %.o
	$(CC) $(CFLAGS) $< xitoa.S -o $@

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

clean :
	rm -f *.hex *.obj *.o

