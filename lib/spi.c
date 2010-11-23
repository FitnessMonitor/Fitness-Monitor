#include "spi.h"

#define PORT_SPI    PORTB
#define DDR_SPI     DDRB
#define DD_MISO     DDB4
#define DD_MOSI     DDB3
#define DD_SS       DDB2
#define DD_SCK      DDB5

void spi_init()
// Initialize pins for spi communication
{
    DDR_SPI &= ~((1<<DD_MOSI)|(1<<DD_MISO)|(1<<DD_SS)|(1<<DD_SCK));
    // Define the following pins as output
    DDR_SPI |= ((1<<DD_MOSI)|(1<<DD_SS)|(1<<DD_SCK));
   
    SPCR = ((1<<SPE)|               // SPI Enable
            (0<<SPIE)|              // SPI Interupt Enable
            (0<<DORD)|              // Data Order (0:MSB first / 1:LSB first)
            (1<<MSTR)|              // Master/Slave select   
<<<<<<< HEAD
            (1<<SPR1)|(1<<SPR0)|    // SPI Clock Rate
=======
            (0<<SPR1)|(1<<SPR0)|    // SPI Clock Rate
>>>>>>> 5793541bd196650923ae10c25b167ddf9128cc26
            (0<<CPOL)|              // Clock Polarity (0:SCK low / 1:SCK hi when idle)
            (0<<CPHA));             // Clock Phase (0:leading / 1:trailing edge sampling)

    SPSR = (1<<SPI2X);              // Double Clock Rate
}

void spi_transfer_array (uint8_t * dataout, uint8_t * datain, uint8_t len)
// Shift full array through target device
{
       uint8_t i;      
       for (i = 0; i < len; i++) {
             SPDR = dataout[i];
             while((SPSR & (1<<SPIF))==0);
             datain[i] = SPDR;
       }
}

void spi_transmit_array (uint8_t * dataout, uint8_t len)
// Shift full array to target device without receiving any byte
{
       uint8_t i;      
       for (i = 0; i < len; i++) {
             SPDR = dataout[i];
             while((SPSR & (1<<SPIF))==0);
       }
}

uint8_t spi_transmit_byte (uint8_t data)
// Clocks only one byte to target device and returns the received one
{
    SPDR = data;
    while((SPSR & (1<<SPIF))==0);
    return SPDR;
}

