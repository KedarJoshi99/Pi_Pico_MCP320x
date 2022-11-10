#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "pico/binary_info.h"
#include "include/MCP320x.hpp"

#define PIN_MISO 8
#define PIN_CS   9
#define PIN_SCK  10
#define PIN_MOSI 11

#define SPI_PORT spi1

int main(){

    stdio_init_all();
    

    // This example will use SPI0 at 0.5MHz.
    spi_init(SPI_PORT, 500 * 1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    // Make the SPI pins available to picotool
    bi_decl(bi_3pins_with_func(PIN_MISO, PIN_MOSI, PIN_SCK, GPIO_FUNC_SPI));

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // Make the CS pin available to picotool
    bi_decl(bi_1pin_with_name(PIN_CS, "SPI CS"));
    
    // Set SPI format
    spi_set_format( SPI_PORT,   // SPI instance
                    8,      // Number of bits per transfer
                    SPI_CPOL_1,      // Polarity (CPOL)
                    SPI_CPHA_1,      // Phase (CPHA)
                    SPI_MSB_FIRST);


    while (true)
    {
        printf("MCP3208 Readings"); 
        for(int i = 0; i < 8; i++){
            int temp = (int)read_adc(SPI_PORT,PIN_CS,i);
            printf("channel %d, adc = %d\n",i,temp); 
        }
        sleep_ms(1000);
    }
    

}