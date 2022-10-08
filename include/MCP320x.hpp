#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "pico/binary_info.h"



static inline void cs_select(int cs) {
    asm volatile("nop \n nop \n nop");
    gpio_put(cs, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect(int cs) {
    asm volatile("nop \n nop \n nop");
    gpio_put(cs, 1);
    asm volatile("nop \n nop \n nop");
}

static int16_t read_adc(spi_inst_t *spi,int _cspin,uint8_t channel) {
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.
   
    uint8_t cmd = ((0x01 << 7) |             // start bit
    (1 << 6) |          // single or differential
    ((channel & 0x07) << 3)); // channel number
    // Buffer to store raw reads
    uint8_t data[2];
   
    cs_select(_cspin);
    spi_write_blocking(spi, &cmd, 1);
    //sleep_ms(10);
    spi_read_blocking(spi, 0, data, 2);
    cs_deselect(_cspin);
    //sleep_ms(10);

    int16_t adc = (int16_t)(data[0] << 7|data[1]);
    return adc;
}
