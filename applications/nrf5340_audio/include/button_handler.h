/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
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

/** \defgroup button_handler Button handler
 *	@{
 */

/** @file
 *  @brief Button handler
 *
 * This module handles, and gives an interface to, buttons.
 *
 * Debouncing of buttons is handled by this module.
 */

#ifndef _BUTTON_HANDLER_H_
#define _BUTTON_HANDLER_H_

#include <stdint.h>
#include <drivers/gpio.h>

/** @brief Button ID
 *
 * The pin number of the button
 */
typedef uint32_t button_pin_t;

/** Button actions
 */
typedef enum { BUTTON_PRESS } button_action_t;

/** Button event
 */
typedef struct {
	button_pin_t button_pin;
	button_action_t button_action;
} button_evt_t;

typedef struct {
	const char *btn_name;
	uint8_t btn_pin;
	uint32_t btn_cfg_mask;
} btn_cfg_t;

/** @brief Initialize button handler, with buttons defined in @ref BUTTONS_LIST.
 *
 * @note This function may only be called once - there is no reinitialize.
 *
 * @return 0 if successful.
 * @return -ENODEV	gpio driver not found
 */
int button_handler_init(void);

/** @brief Check button state.
 *
 * @param[in] button_pin Button pin
 * @param[out] button_pressed Button state. True if currently pressed, false otherwise
 *
 * @return 0 if success, an error code otherwise.
 */
int button_pressed(gpio_pin_t button_pin, bool *button_pressed);

#endif /* _BUTTON_HANDLER_H_ */

/** @} */ /* End defgroup */
