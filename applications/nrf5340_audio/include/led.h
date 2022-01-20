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

#ifndef _LED_H_
#define _LED_H_

#include <stdint.h>

#define RED   0
#define GREEN 1
#define BLUE  2

#define GRN GREEN
#define BLU BLUE

typedef enum {
	LED_COLOR_OFF, /* 000 */
	LED_COLOR_RED, /* 001 */
	LED_COLOR_GREEN, /* 010 */
	LED_COLOR_YELLOW, /* 011 */
	LED_COLOR_BLUE, /* 100 */
	LED_COLOR_MAGENTA, /* 101 */
	LED_COLOR_CYAN, /* 110 */
	LED_COLOR_WHITE, /* 111 */
	LED_COLOR_NUM,
} led_color_t;

#define LED_ON LED_COLOR_WHITE

#define LED_BLINK true
#define LED_SOLID false

/**
 * @brief Set the state of a given LED unit to blink.
 *
 * @note A led unit is defined as an RGB LED or a monochrome LED.
 *
 * @param led_unit	Selected LED unit. Defines are located in board.h
 * @note		If the given LED unit is an RGB LED, color must be
 *			provided as a single vararg. See led_color_t.
 *			For monochrome LEDs, the vararg will be ignored.
 * @return		0 on success
 *			-ENXIO if the given led unit is assigned to another core
 *			-EPERM if the module has not been initialised
 *			-EINVAL if the color argument is illegal
 *			Other errors from underlying drivers.
 */
int led_blink(uint8_t led_unit, ...);

/**
 * @brief Turn the given LED unit on.
 *
 * @note A led unit is defined as an RGB LED or a monochrome LED.
 *
 * @param led_unit	Selected LED unit. Defines are located in board.h
 * @note		If the given LED unit is an RGB LED, color must be
 *			provided as a single vararg. See led_color_t.
 *			For monochrome LEDs, the vararg will be ignored.
 * @return		0 on success
 *			-ENXIO if the given led unit is assigned to another core
 *			-EPERM if the module has not been initialised
 *			-EINVAL if the color argument is illegal
 *			Other errors from underlying drivers.
 */
int led_on(uint8_t led_unit, ...);

/**
 * @brief Set the state of a given LED unit to off.
 *
 * @note A led unit is defined as an RGB LED or a monochrome LED.
 *
 * @param led_unit	Selected LED unit. Defines are located in board.h
 * @return		0 on success
 *			-ENXIO if the given led unit is assigned to another core
 *			-EPERM if the module has not been initialised
 *			-EINVAL if the color argument is illegal
 *			Other errors from underlying drivers.
 */
int led_off(uint8_t led_unit);

/**
 * @brief Initialise the LED module
 *
 * @note This will parse the .dts files and configure all LEDs.
 *
 * @return	0 on success
 *		-EPERM if already initialsed
 *		-ENXIO if a LED is missing unit number in dts
 *		-ENODEV if a LED is missing core specifier or
 *		color identifier
 */
int led_init(void);

#endif /* _LED_H_ */
