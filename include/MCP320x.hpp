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

uint8_t ch0 = 0B10000000;
#define B10000000 0x80 ///0
#define B10010000 0x90 ///1
#define B10100000 0xA0 ///2
#define B10110000 0xB0 ///3
#define B11000000 0xC0 ///4
#define B11010000 0xD0 ///5
#define B11100000 0xE0 ///6
#define B11110000 0xF0 ///7




/* MCP320X Class * ===========================================================================> 
*  See MCP320X.cpp for more details
* ============================================================================================> */	
class MCP320X
{
	public:
		MCP320X(uint8_t model, uint8_t _spi, uint8_t cspin);
		MCP320X(uint8_t model, uint8_t _spi, uint8_t clockpin, uint8_t mosipin, uint8_t misopin, uint8_t cspin);
		~MCP320X();
		uint8_t* readADC(uint8_t ch);
		
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
    this->_pinNo = model; // the model id (X) defines the number of adc inputs
    this->_cspin = cspin;
    if(_spi == 0){
		this->_clockpin = PICO_DEFAULT_SPI_SCK_PIN;
		this->_mosipin = PICO_DEFAULT_SPI_TX_PIN;
		this->_misopin = PICO_DEFAULT_SPI_RX_PIN;
	}
	else{
		this->spi = spi1;
		this->_clockpin = 14;
		this->_mosipin = 15;
		this->_misopin = 12;
	}

    
// This example will use SPI0 at 0.5MHz.
    spi_init(this->spi, 500 * 1000);
    gpio_set_function(this->_misopin, GPIO_FUNC_SPI);
    gpio_set_function(this->_clockpin, GPIO_FUNC_SPI);
    gpio_set_function(this->_mosipin, GPIO_FUNC_SPI);
    // Make the SPI pins available to picotool
    bi_decl(bi_3pins_with_func(this->_misopin, this->_mosipin, this->_clockpin, GPIO_FUNC_SPI));

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(this->_cspin);
    gpio_set_dir(this->_cspin, GPIO_OUT);
    gpio_put(this->_cspin, 1);
    // Make the CS pin available to picotool
    bi_decl(bi_1pin_with_name(this->_cspin, "SPI CS"));
    
}
MCP320X::MCP320X(uint8_t model, uint8_t _spi, uint8_t clockpin, uint8_t mosipin, uint8_t misopin, uint8_t cspin){

	this->_pinNo = model; 
    this->_cspin = cspin;
    this->_clockpin = clockpin;
    this->_mosipin = mosipin;
    this->_misopin = misopin;
	if(_spi == 0){
		this->spi = spi0;
	}
	else{
		this->spi = spi1;
	}

	// This example will use SPI0 at 0.5MHz.
    spi_init(this->spi, 500 * 1000);
    gpio_set_function(this->_misopin, GPIO_FUNC_SPI);
    gpio_set_function(this->_clockpin, GPIO_FUNC_SPI);
    gpio_set_function(this->_mosipin, GPIO_FUNC_SPI);

    // Make the SPI pins available to picotool
    bi_decl(bi_3pins_with_func(this->_misopin, this->_mosipin, this->_clockpin, GPIO_FUNC_SPI));

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(this->_cspin);
    gpio_set_dir(this->_cspin, GPIO_OUT);
    gpio_put(this->_cspin, 1);

    // Make the CS pin available to picotool
    bi_decl(bi_1pin_with_name(this->_cspin, "SPI CS"));
}

MCP320X::~MCP320X(){}



uint8_t* MCP320X::readADC(uint8_t channel)
{
	uint8_t buffer[26];
	uint8_t cmd = ((0x01 << 7) |             // start bit
            (1 << 6) |          // single or differential
            ((channel & 0x07) << 3)); // channel number

	read_registers(cmd, buffer, 24);	
	return buffer;
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
    spi_read_blocking(spi, reg, buf, len);
    cs_deselect();
    sleep_ms(10);
}


