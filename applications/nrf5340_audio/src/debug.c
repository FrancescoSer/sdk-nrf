/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * SPDX-License-Identifier: Nordic-5-Clause
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
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
