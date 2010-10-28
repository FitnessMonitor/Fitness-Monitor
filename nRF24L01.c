#include "nRF24L01.h"
#include "spi.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// Defines for setting the nRF24L01 registers for transmitting or receiving mode
#define TX_POWERUP nRF24L01_config_register(CONFIG, nRF24L01_CONFIG | ( (1<<PWR_UP) | (0<<PRIM_RX) ) )
#define RX_POWERUP nRF24L01_config_register(CONFIG, nRF24L01_CONFIG | ( (1<<PWR_UP) | (1<<PRIM_RX) ) )
#define PWR_DOWN nRF24L01_config_register(CONFIG, nRF24L01_CONFIG | ( (0<<PWR_UP) | (0<<PRIM_RX) ) )

// Flag which denotes transmitting mode
volatile uint8_t PTX;

uint8_t nRF24L01_data[32];
uint8_t *buffer =  &nRF24L01_data[0];
uint8_t buffersize = sizeof(nRF24L01_data);

void nRF24L01_init() 
// Initializes pins ans interrupt to communicate with the nRF24L01 module
// Should be called in the early initializing phase at startup.
{
	// Define CSN and CE as Output and set them to default
	DDRB |= ((1<<CSN)|(1<<CE));
	nRF24L01_CE_lo;
	nRF24L01_CSN_hi;

	// Initialize external interrupt on port PD6 (PCINT22)
	DDRD &= ~(1<<PD6);
	PCMSK2 = (1<<PCINT22);
	PCICR  = (1<<PCIE2);

	// Initialize spi module
	spi_init();
}

void nRF24L01_config() 
// Sets the important registers in the nRF24L01 module and powers the module
// in receiving mode
{
	// Set RF channel
	nRF24L01_config_register(RF_CH,nRF24L01_CH);

	// Set length of incoming payload 
	nRF24L01_config_register(RX_PW_P0, nRF24L01_PAYLOAD);
	
	//configure 1Mbps	
	uint8_t nRF24L01_PWR_RATE =  ( (0<<LNA_HCURR) | (00<RF_PWR) | (0<<RF_DR)); // 18dbm & 1Mbps
	nRF24L01_write_register(RF_SETUP, &nRF24L01_PWR_RATE,1);

	// Reset status register
	nRF24L01_config_register(STATUS,((1<<TX_DS)|(1<<MAX_RT))); 

	// Start receiver 
	PTX = 0;        	// Start in receiving mode
	RX_POWERUP;     	// Power up in receiving mode
	nRF24L01_CE_hi;		// Listening for pakets
	//nRF24L01_CE_lo;
}


void nRF24L01_set_RADDR(uint8_t * adr) 
// Sets the receiving address
{
	nRF24L01_CE_lo;
	nRF24L01_write_register(RX_ADDR_P0,adr,5);
    	nRF24L01_CE_hi;
}

void nRF24L01_set_TADDR(uint8_t * adr)
// Sets the transmitting address
{
    	nRF24L01_write_register(TX_ADDR, adr,5);
}


extern void nRF24L01_interrupt () 
{
        // Read nRF24L01 status 
	uint8_t status;
        nRF24L01_CSN_lo;			// Pull down chip select
        status = spi_transmit_byte(NOP);	// Read status register
        nRF24L01_CSN_hi;			// Pull up chip select
	
	//based upon the status register decide what to do


	//if TX sucess
	if ( status & (1<<TX_DS) )
	{
		nRF24L01_powerdown();		// Return to low power state
	}
	//maximum retrys reached
	else if ( status & (1<<MAX_RT) )
	{
		//set some flag
		nRF24L01_powerdown();		// Return to low power state
	}
	//recieved a packet
	else if ( status & (1<<RX_DR) )
	{
		nRF24L01_get_data(buffer);
		nRF24L01_powerdown();
	}

        // Reset status register for further interaction
        nRF24L01_config_register(STATUS,( (1<<RX_DR) | (1<<TX_DS) | (1<<MAX_RT) ) ); // Reset status register

}

extern void nRF24L01_powerdown()
//place nRF24L01 in powerdown mode
{
	//empty the TX FIFO
	nRF24L01_CSN_lo;			// Pull down chip select
    	spi_transmit_byte( FLUSH_TX );		// Write cmd to flush tx fifo
   	nRF24L01_CSN_hi;			// Pull up chip select

        nRF24L01_CE_lo;				// Deactivate transceiver
	PWR_DOWN;				// power down transceiver
        nRF24L01_CE_hi;				// 
}
extern void nRF24L01_RX_powerup()
//place nRF24L01 in recieve mode
{
        nRF24L01_CE_lo;				// Deactivate transceiver
	RX_POWERUP;				// power down transceiver
        nRF24L01_CE_hi;				// Listening for pakets
}


extern uint8_t nRF24L01_data_ready() 
// Checks if data is available for reading
{
	// Read nRF24L01 status 
	uint8_t status;
	nRF24L01_CSN_lo;				// Pull down chip select
	status = spi_transmit_byte(NOP);		// Read status register
	nRF24L01_CSN_hi;				// Pull up chip select
	return status & (1<<RX_DR);
}

extern void nRF24L01_get_data(uint8_t * data) 
// Reads nRF24L01_PAYLOAD bytes into data array
{
	
	nRF24L01_CSN_lo;				// Pull down chip select
	nRF24L01_CE_lo;
	spi_transmit_byte( R_RX_PAYLOAD );		// Send cmd to read rx payload
	spi_transfer_array(data,data,nRF24L01_PAYLOAD); // Read payload
	nRF24L01_CSN_hi;				// Pull up chip select
	nRF24L01_config_register(STATUS,(1<<RX_DR));	// Reset status register
	nRF24L01_CE_hi;

	//Clear the TX FIFO
	nRF24L01_CSN_lo;				// Pull down chip select
	spi_transmit_byte( FLUSH_TX );			// Write cmd to flush tx fifo
	nRF24L01_CSN_hi;				// Pull up chip select
}

void nRF24L01_config_register(uint8_t reg, uint8_t value)
// Clocks only one byte into the given nRF24L01 register
{
	nRF24L01_CSN_lo;
	spi_transmit_byte(W_REGISTER | (REGISTER_MASK & reg));
	spi_transmit_byte(value);
	nRF24L01_CSN_hi;
}

void nRF24L01_read_register(uint8_t reg, uint8_t * value, uint8_t len)
// Reads an array of bytes from the given start position in the nRF24L01 registers.
{
	nRF24L01_CSN_lo;
	spi_transmit_byte(R_REGISTER | (REGISTER_MASK & reg));
	spi_transfer_array(value,value,len);
	nRF24L01_CSN_hi;
}

void nRF24L01_write_register(uint8_t reg, uint8_t * value, uint8_t len) 
// Writes an array of bytes into inte the nRF24L01 registers.
{
	nRF24L01_CSN_lo;
	spi_transmit_byte(W_REGISTER | (REGISTER_MASK & reg));
	spi_transmit_array(value,len);
	nRF24L01_CSN_hi;
}

void nRF24L01_send(uint8_t * value, uint8_t len) 
// Sends a data package to the default address. Be sure to send the correct
// amount of bytes as configured as payload on the receiver.
{
	nRF24L01_CE_lo;

	TX_POWERUP;				// Power up
    
	nRF24L01_CSN_lo;			// Pull down chip select
	spi_transmit_byte( FLUSH_TX );		// Write cmd to flush tx fifo
	nRF24L01_CSN_hi;			// Pull up chip select
    
	nRF24L01_CSN_lo;			// Pull down chip select
	spi_transmit_byte( W_TX_PAYLOAD );	// Write cmd to write payload
	spi_transmit_array(value,len);		// Write payload
	nRF24L01_CSN_hi;			// Pull up chip select
    
	nRF24L01_CE_hi;				// Start transmission
}

