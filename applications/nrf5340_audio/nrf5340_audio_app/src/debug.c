/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <drivers/gpio.h>
#include <hal/nrf_gpio.h>

static uint32_t debug_spi_clk_pin;
static uint32_t debug_spi_mosi_pin;

static inline void debug_spi_wait(void)
{
	NRF_P1->DIRSET = NRF_P1->DIRSET;
	NRF_P1->DIRSET = NRF_P1->DIRSET;
}

/**@brief Set up what two pins to use for sending debug data over SPI
 *
 * @param clk_pin	Pin used for clock
 * @param mosi_pin	Pin used for sending data
 *
 * @note		Send pins using BIT(<PIN_NUMBER>)
 */
void debug_spi_config(uint32_t clk_pin, uint32_t mosi_pin)
{
	debug_spi_clk_pin = clk_pin;
	debug_spi_mosi_pin = mosi_pin;

	nrf_gpio_cfg_output(debug_spi_clk_pin);
	nrf_gpio_cfg_output(debug_spi_mosi_pin);

	nrf_gpio_pin_clear(debug_spi_clk_pin);
	nrf_gpio_pin_clear(debug_spi_mosi_pin);
}

/**@brief Send single byte using SPI
 *
 * @param byte to send
 *
 * @note: Usage: debug_spi_write_byte('t');
 */
void debug_spi_write_byte(uint8_t byte)
{
	for (int i = 7; i >= 0; i--) {
		if ((byte >> i) & 1) {
			nrf_gpio_pin_set(debug_spi_mosi_pin);
		} else {
			nrf_gpio_pin_clear(debug_spi_mosi_pin);
		}

		debug_spi_wait();
		nrf_gpio_pin_set(debug_spi_clk_pin);
		debug_spi_wait();
		nrf_gpio_pin_clear(debug_spi_clk_pin);
	}

	nrf_gpio_pin_clear(debug_spi_mosi_pin);
}

/**@brief Write string over SPI
 *
 * @param string	String to write
 *
 * @note Usage: debug_spi_write_string("string to write");
 */
void debug_spi_write_string(char *string)
{
	uint32_t i = 0;

	while (string[i] != '\0') {
		debug_spi_write_byte(string[i]);
		i++;
	}
}

/**@brief Write number using SPI
 *
 * @param num	Number to write
 * @param base	Base to print number as
 *
 * @note Usage debug_spi_write_number(28363, 16); to get 6ECB on SPI
 */
void debug_spi_write_number(uint32_t num, uint8_t base)
{
	uint32_t divisor = 1;

	while (num >= (divisor * base)) {
		divisor *= base;
	}

	while (divisor) {
		uint8_t value = (uint8_t)(num / divisor);
		uint8_t c = value + '0';

		if (c > '9') {
			c += 'A' - '9' - 1;
		}
		debug_spi_write_byte(c);

		num -= value * divisor;
		divisor /= base;
	}
}
