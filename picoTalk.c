#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "ssd1306/ssd1306.h"
#include "st7789v/st7789v.h"

#define SPI_PORT spi0
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5


int main() {
    stdio_init_all();
    i2c_inst_t *i2c = i2c0;
    spi_inst_t *spi = spi0;

    ssd1306_init(i2c, I2C_SDA, I2C_SCL, SSD1306_I2C_ADDR);
    // st7789v_init(spi, PIN_CS, PIN_SCK, PIN_MOSI);

    ssd1306_blink(10, 50);
}