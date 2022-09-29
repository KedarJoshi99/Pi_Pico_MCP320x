#ifndef MCP320X_h
#define MCP320X_h

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "pico/binary_info.h"


/* MCP320X Class * ===========================================================================> 
*  See MCP320X.cpp for more details
* ============================================================================================> */	
class MCP320X
{
	public:
		MCP320X(uint8_t model, uint8_t _spi, uint8_t cspin);
		MCP320X(uint8_t model, uint8_t _spi, uint8_t clockpin, uint8_t mosipin, uint8_t misopin, uint8_t cspin);
		~MCP320X();
		
		int16_t readADC(uint8_t ch);
		
	private:
		  uint8_t _pinNo, _clockpin, _mosipin, _misopin, _cspin; 
		  spi_inst_t *spi;
		  void cs_select();
		  void cs_deselect();
		  void read_registers(uint8_t reg, uint8_t *buf, uint16_t len);
		  

};
#endif 

/* MCP320X() =================================================================================>
*	Constructor function.
*	Parameters:
*		* model: 	The 'X' part of the model identifier (i.e. MCP3204 -> model=4)
*		* clockpin: 	The SPI clock pin (CSK) of your choise.
*		* mosipin:	The SPI MOSI pin of your choice.
*		* misopin:	The SPI MISO pin of your choice.
*		* cspin:	The SPI CS pin of your choice.
* ============================================================================================> */
MCP320X::MCP320X(uint8_t model, uint8_t _spi, uint8_t cspin) {
    
    // define SPI outputs and inputs for bitbanging
    _pinNo = model; // the model id (X) defines the number of adc inputs
    _cspin = cspin;
    if(_spi == 0){
		_clockpin = PICO_DEFAULT_SPI_SCK_PIN;
		_mosipin = PICO_DEFAULT_SPI_TX_PIN;
		_misopin = PICO_DEFAULT_SPI_RX_PIN;
	}
	else{
		spi = spi1;
		_clockpin = 14;
		_mosipin = 15;
		_misopin = 12;
	}

	
    
// This example will use SPI0 at 0.5MHz.
    spi_init(spi, 500 * 1000);
    gpio_set_function(_misopin, GPIO_FUNC_SPI);
    gpio_set_function(_clockpin, GPIO_FUNC_SPI);
    gpio_set_function(_mosipin, GPIO_FUNC_SPI);
    // // Make the SPI pins available to picotool
    // bi_decl(bi_3pins_with_func(_misopin, _mosipin, _clockpin, GPIO_FUNC_SPI));

    spi_set_format( spi,   // SPI instance
                    8,      // Number of bits per transfer
                    SPI_CPOL_1,      // Polarity (CPOL)
                    SPI_CPHA_1,      // Phase (CPHA)
                    SPI_MSB_FIRST);

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(_cspin);
    gpio_set_dir(_cspin, GPIO_OUT);
    gpio_put(_cspin, 1);
    // // Make the CS pin available to picotool
    // bi_decl(bi_1pin_with_name(_cspin, "SPI CS"));
    
}
MCP320X::MCP320X(uint8_t model, uint8_t _spi, uint8_t clockpin, uint8_t mosipin, uint8_t misopin, uint8_t cspin){

	_pinNo = model; 
    _cspin = cspin;
    _clockpin = clockpin;
    _mosipin = mosipin;
    _misopin = misopin;
	if(_spi == 0){
		spi = spi0;
	}
	else{
		spi = spi1;
	}

	

	// This example will use SPI0 at 0.5MHz.
    spi_init(spi, 500 * 1000);
    gpio_set_function(_misopin, GPIO_FUNC_SPI);
    gpio_set_function(_clockpin, GPIO_FUNC_SPI);
    gpio_set_function(_mosipin, GPIO_FUNC_SPI);

	spi_set_format( spi,   // SPI instance
                    8,      // Number of bits per transfer
                    SPI_CPOL_1,      // Polarity (CPOL)
                    SPI_CPHA_1,      // Phase (CPHA)
                    SPI_MSB_FIRST);

    // // Make the SPI pins available to picotool
    // bi_decl(bi_3pins_with_func(_misopin, _mosipin, _clockpin, GPIO_FUNC_SPI));

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(_cspin);
    gpio_set_dir(_cspin, GPIO_OUT);
    gpio_put(_cspin, 1);

    // // Make the CS pin available to picotool
    // bi_decl(bi_1pin_with_name(_cspin, "SPI CS"));
}

MCP320X::~MCP320X(){}



int16_t MCP320X::readADC(uint8_t channel)
{
	uint8_t buffer[2];
	uint8_t cmd = ((0x01 << 7) |             // start bit
            (1 << 6) |          // single or differential
            ((channel & 0x07) << 3)); // channel number

	read_registers(cmd, buffer, 2);
	read_registers(cmd, buffer, 2);	
	int16_t adc = (int16_t)(buffer[0]|buffer[1]);
	adc = 0;
	return adc;
}

void MCP320X::cs_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(_cspin, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

void MCP320X::cs_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(_cspin, 1);
    asm volatile("nop \n nop \n nop");
}

void MCP320X::read_registers(uint8_t reg, uint8_t *buf, uint16_t len) {
    cs_select();
	spi_write_blocking(spi, &reg, 1);
    spi_read_blocking(spi, 0, buf, len);
    cs_deselect();
    sleep_ms(1);
}


