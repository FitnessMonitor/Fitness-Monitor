CC=avr-gcc
CFLAGS=-g -Os -Wall -mcall-prologues -mmcu=atmega168
OBJ2HEX=avr-objcopy 
UISP=uisp 
TARGET=main

program : $(TARGET).hex
	avrdude -c usbtiny -p m168 -U flash:w:main.hex
%.obj : %.o
	$(CC) $(CFLAGS) $< -o $@

%.hex : %.obj
	$(OBJ2HEX) -R .eeprom -O ihex $< $@

clean :
	rm -f *.hex *.obj *.o

