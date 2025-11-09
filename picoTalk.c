#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "ssd1306/ssd1306.h"
// #include "st7789v/st7789v.h"

// ST7789V Pin Defines
#define SPI_PORT spi0
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

// SSD1306 Pin Defines
#define I2C_PORT i2c1
#define I2C_SDA 27
#define I2C_SCL 26

// Button Matrix Pin Defines
#define OUT1 13
#define OUT2 14
#define OUT3 15
#define IN1 1
#define IN2 5
#define IN3 8
#define IN4 11

#define INPUT false
#define OUTPUT true
#define LOW 0
#define HIGH 1

void matrixInit(void) {
    // Initialize column pins as outputs and drive HIGH (idle)
    gpio_init(OUT1);
    gpio_init(OUT2);
    gpio_init(OUT3);

    gpio_set_dir(OUT1, OUTPUT);
    gpio_set_dir(OUT2, OUTPUT);
    gpio_set_dir(OUT3, OUTPUT);

    gpio_put(OUT1, HIGH);
    gpio_put(OUT2, HIGH);
    gpio_put(OUT3, HIGH);

    // Initialize input (row) pins with pull-ups
    gpio_init(IN1);
    gpio_init(IN2);
    gpio_init(IN3);
    gpio_init(IN4);

    gpio_set_dir(IN1, INPUT);
    gpio_set_dir(IN2, INPUT);
    gpio_set_dir(IN3, INPUT);
    gpio_set_dir(IN4, INPUT);

    gpio_pull_up(IN1);
    gpio_pull_up(IN2);
    gpio_pull_up(IN3);
    gpio_pull_up(IN4);
}

uint8_t returnKeyPressed(void) {
    const uint8_t col_map[3][4] = {
        { '1', '2', '3', '*' },   // OUT1
        { '4', '5', '6', '0' },   // OUT2
        { '7', '8', '9', '#' }    // OUT3
    };

    uint8_t key = '\0';

    // Ensure all columns idle HIGH before scanning
    gpio_put(OUT1, HIGH);
    gpio_put(OUT2, HIGH);
    gpio_put(OUT3, HIGH);

    for (int c = 0; c < 3 && key == '\0'; ++c) {
        uint8_t col_pin = (c == 0) ? OUT1 : (c == 1) ? OUT2 : OUT3;
        gpio_put(col_pin, LOW);

        sleep_us(200);

        // Read rows
        uint8_t r1 = gpio_get(IN1);
        uint8_t r2 = gpio_get(IN2);
        uint8_t r3 = gpio_get(IN3);
        uint8_t r4 = gpio_get(IN4);

        // If any row reads LOW, confirm with debounce
        if (r1 == 0 || r2 == 0 || r3 == 0 || r4 == 0) {
            sleep_ms(15);
            // Re-read same rows
            if ((gpio_get(IN1) == 0 && r1 == 0) || (gpio_get(IN2) == 0 && r2 == 0) || (gpio_get(IN3) == 0 && r3 == 0) || (gpio_get(IN4) == 0 && r4 == 0)) {
                if (gpio_get(IN1) == 0) key = col_map[c][0];
                else if (gpio_get(IN2) == 0) key = col_map[c][1];
                else if (gpio_get(IN3) == 0) key = col_map[c][2];
                else if (gpio_get(IN4) == 0) key = col_map[c][3];

                // Wait for release of the same row
                while ((key == col_map[c][0] && gpio_get(IN1) == 0) || (key == col_map[c][1] && gpio_get(IN2) == 0) || (key == col_map[c][2] && gpio_get(IN3) == 0) || (key == col_map[c][3] && gpio_get(IN4) == 0)) tight_loop_contents();
            }
        }

        // Return column to idle HIGH
        gpio_put(col_pin, HIGH);

        sleep_us(50);
    }

    return key;
}

void main(void) {
    stdio_init_all();
    i2c_inst_t *i2c = I2C_PORT;
    spi_inst_t *spi = spi0;
    uint8_t textBuff[10];

    ssd1306_init(i2c, I2C_SDA, I2C_SCL, SSD1306_I2C_ADDR);
    // st7789v_init(spi, PIN_CS, PIN_SCK, PIN_MOSI);
    matrixInit();

    ssd1306_blink(10, 50);

    uint8_t i = 0;
    while (true) {
        uint8_t key = returnKeyPressed();
        if (key != '\0') {
            textBuff[i % 10] = key;
            ssd1306_write_text(textBuff, 5, 5);
            ssd1306_update();
            i++;
        }
    }
}