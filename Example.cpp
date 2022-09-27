#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "include/MCP320x.hpp"

#define CS_PIN 17     // ESP8266 default SPI pins
#define CLOCK_PIN 18  // Should work with any other GPIO pins, since the library does not formally
#define MOSI_PIN 19   // use SPI, but rather performs pin bit banging to emulate SPI communication.
#define MISO_PIN 16   //
#define MCP320x 8     // (Generally "#define MCP320X X", where X is the last model digit/number of inputs)



int main(){

    MCP320X mcp3208(MCP320x, CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN);
    while(1){
        printf("MCP3208: ADC Values are:\n");
        for(int i = 0; i < 8; i ++){
            printf("Channel %d: %d\n", i,mcp3208.readADC(i));
        }
        sleep_ms(1000);
    }
}