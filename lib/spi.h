#ifndef _SPI_H_
#define _SPI_H_

//function Prototypes
extern void spi_init();
extern void spi_transfer_array (uint8_t * dataout, uint8_t * datain, uint8_t len);
extern void spi_transmit_array (uint8_t * dataout, uint8_t len);
extern uint8_t spi_transmit_byte (uint8_t data);

#endif /* _SPI_H_ */
