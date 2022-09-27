#ifndef MCP320X_h
#define MCP320X_h

#include <string.h>
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
		MCP320X(uint8_t model, uint8_t clockpin, uint8_t mosipin, uint8_t misopin, uint8_t cspin);
		~MCP320X();
		int16_t readADC(uint8_t ch);
		
	private:
		  uint8_t _pinNo, _clockpin, _mosipin, _misopin, _cspin;
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
MCP320X::MCP320X(uint8_t model, uint8_t clockpin, uint8_t mosipin, uint8_t misopin, uint8_t cspin) {
    
    // define SPI outputs and inputs for bitbanging
    _pinNo = model; // the model id (X) defines the number of adc inputs
    _cspin = cspin;
    _clockpin = clockpin;
    _mosipin = mosipin;
    _misopin = misopin;
    
	gpio_init(_cspin);
	gpio_init(_clockpin);
	gpio_init(_mosipin);
	gpio_init(_misopin);
    gpio_set_dir(_cspin, GPIO_OUT);
    gpio_set_dir(_clockpin, GPIO_OUT);
    gpio_set_dir(_mosipin, GPIO_OUT);
    gpio_set_dir(_misopin, GPIO_IN);
    
}


/* ~MCP320X() ================================================================================>
*	Destructor function
* ============================================================================================> */
MCP320X::~MCP320X(){}


/* readADC() =================================================================================>
*	Function to read from a channel.
*	Parameters:
*		* channel: The channel number to read from (MAX=_pinNo-1)
*	Returns:
*		* The ADC value of a given channel (0-VREF -> 0-4095), if a valid channel is given.
*		* -1 if the channel id is less than 0 or exceeds the pin number for the defined model.
* ============================================================================================> */
int16_t MCP320X::readADC(uint8_t channel)
{
	// If an invalid channel is passed, return -1
	if ((channel >= _pinNo) || (channel < 0)) return -1; 
	
	int adcvalue = 0;
	uint8_t cmd = 0xC0; //B11000000; //command bits - start, mode, chn (3), dont care (3)

	// allow channel selection
	cmd|=((channel)<<3);

	gpio_put(_cspin, 0); //Select adc
	// setup bits to be written
	for (int i=7; i>=3; i--){
		gpio_put(_mosipin, cmd & 1<<i);
		//cycle clock
		gpio_put(_clockpin, 1);
		gpio_put(_clockpin, 0);    
	}

	gpio_put(_clockpin, 1);    //ignores 2 null bits
	gpio_put(_clockpin, 0);
	gpio_put(_clockpin, 1);  
	gpio_put(_clockpin, 0);

	// read bits from adc
	for (int i=11; i>=0; i--){
		adcvalue+=gpio_get(_misopin)<<i;
		//cycle clock
		gpio_put(_clockpin, 1);
		gpio_put(_clockpin, 0);
	}
	gpio_put(_cspin, 1); //turn off device
	
	// return the result
	return adcvalue;
}

