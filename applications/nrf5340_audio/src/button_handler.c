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

#include "button_handler.h"
#include "button_assignments.h"

#include <zephyr.h>
#include <drivers/gpio.h>
#include <sys/util.h>
#include <shell/shell.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "macros_common.h"
#include "ctrl_events.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(button_handler, CONFIG_LOG_BUTTON_HANDLER_LEVEL);

/* How many buttons does the module support. Increase at memory cost */
#define BUTTONS_MAX 5
#define BASE_10	    10

static bool debounce_is_ongoing;
static struct gpio_callback btn_callback[BUTTONS_MAX];

const static btn_cfg_t btn_cfg[] = {
#define X(_name, _dts_alias) \
	{ \
		.btn_name = STRINGIFY(_name), \
		.btn_pin = DT_GPIO_PIN(DT_ALIAS(_dts_alias), gpios), \
		.btn_cfg_mask = DT_GPIO_FLAGS(DT_ALIAS(_dts_alias), gpios), \
	},
	BUTTONS_LIST
#undef X
};

static const struct device *gpio_53_dev;

/**@brief Simple debouncer for buttons
 *
 * @note Needed as low-level driver debouce is not
 * implemented in Zephyr for nRF53 yet
 */
static void on_button_debounce_timeout(struct k_timer *timer)
{
	debounce_is_ongoing = false;
}

K_TIMER_DEFINE(button_debounce_timer, on_button_debounce_timeout, NULL);

/** @brief Find the index of a button from the pin number
 */
static int pin_to_btn_idx(uint8_t btn_pin, uint32_t *pin_idx)
{
	for (uint8_t i = 0; i < ARRAY_SIZE(btn_cfg); i++) {
		if (btn_pin == btn_cfg[i].btn_pin) {
			*pin_idx = i;
			return 0;
		}
	}

	LOG_WRN("Button idx not found");
	return -ENODEV;
}

/** @brief Convert from mask to pin
 *
 * @note: Will check that a single bit and a single bit only is set in the mask.
 */
static int pin_msk_to_pin(uint32_t pin_msk, uint32_t *pin_out)
{
	if (!pin_msk) {
		RET_IF_ERR_MSG(-EACCES, "Mask is empty");
	}

	if (pin_msk & (pin_msk - 1)) {
		RET_IF_ERR_MSG(-EACCES, "Two or more buttons set in mask");
	}

	*pin_out = 0;

	while (pin_msk) {
		pin_msk = pin_msk >> 1;
		(*pin_out)++;
	}

	/* Deduct 1 for zero indexing */
	(*pin_out)--;

	return 0;
}

/*  ISR triggered by GPIO when assigned button(s) are pushed */
static void button_isr(const struct device *port, struct gpio_callback *cb, uint32_t pin_msk)
{
	int ret;
	struct event_t event;

	if (debounce_is_ongoing) {
		LOG_WRN("Btn debounce in action");
		return;
	}

	uint32_t btn_pin = 0;
	uint32_t btn_idx = 0;

	ret = pin_msk_to_pin(pin_msk, &btn_pin);
	ERR_CHK(ret);

	ret = pin_to_btn_idx(btn_pin, &btn_idx);
	ERR_CHK(ret);

	LOG_DBG("Pushed button idx: %d pin: %d name: %s", btn_idx, btn_pin,
		btn_cfg[btn_idx].btn_name);

	event.button_activity.button_pin = btn_pin;
	event.button_activity.button_action = BUTTON_PRESS;
	event.event_source = EVT_SRC_BUTTON;

	/* To avoid filling up the event queue with button presses,
	 * we only allow button events if all other events have been processed
	 */
	if (ctrl_events_queue_empty()) {
		ret = ctrl_events_put(&event);
		ERR_CHK(ret);
		debounce_is_ongoing = true;
		k_timer_start(&button_debounce_timer, K_MSEC(CONFIG_BUTTON_DEBOUNCE_MS), K_NO_WAIT);
	} else {
		LOG_WRN("Event queue is not empty, try again later");
	}
}

int button_pressed(gpio_pin_t button_pin, bool *button_pressed)
{
	int ret;

	if (!gpio_53_dev) {
		return -ENODEV;
	}

	if (button_pressed == NULL) {
		return -EINVAL;
	}

	ret = gpio_pin_get(gpio_53_dev, button_pin);
	switch (ret) {
	case 0:
		*button_pressed = false;
		break;
	case 1:
		*button_pressed = true;
		break;
	default:
		return ret;
	}

	return 0;
}

int button_handler_init(void)
{
	int ret;

	if (ARRAY_SIZE(btn_cfg) == 0) {
		LOG_WRN("No buttons assigned");
		return -EINVAL;
	}

	gpio_53_dev = device_get_binding("GPIO_0");

	if (!gpio_53_dev) {
		LOG_ERR("Device driver not found.");
		return -ENODEV;
	}

	for (uint8_t i = 0; i < ARRAY_SIZE(btn_cfg); i++) {
		ret = gpio_pin_configure(gpio_53_dev, btn_cfg[i].btn_pin, btn_cfg[i].btn_cfg_mask);
		RET_IF_ERR(ret);

		gpio_init_callback(&btn_callback[i], button_isr, BIT(btn_cfg[i].btn_pin));
		RET_IF_ERR(ret);

		ret = gpio_add_callback(gpio_53_dev, &btn_callback[i]);
		RET_IF_ERR(ret);

		ret = gpio_pin_interrupt_configure(gpio_53_dev, btn_cfg[i].btn_pin,
						   GPIO_INT_EDGE_TO_INACTIVE);
		RET_IF_ERR(ret);
	}

	return 0;
}

/* Shell functions */
static int cmd_print_all_btns(const struct shell *shell, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	for (uint8_t i = 0; i < ARRAY_SIZE(btn_cfg); i++) {
		shell_print(shell, "Id %d: pin: %d %s", i, btn_cfg[i].btn_pin, btn_cfg[i].btn_name);
	}

	return 0;
}

static int cmd_push_btn(const struct shell *shell, size_t argc, char **argv)
{
	int ret;
	uint8_t btn_idx;
	struct event_t event;

	/* First argument is function, second is button idx */
	if (argc != 2) {
		shell_error(shell, "Wrong number of arguments provided");
		return -EINVAL;
	}

	if (!isdigit((int)argv[1][0])) {
		shell_error(shell, "Supplied argument is not numeric");
		return -EINVAL;
	}

	btn_idx = strtoul(argv[1], NULL, BASE_10);

	if (btn_idx >= ARRAY_SIZE(btn_cfg)) {
		shell_error(shell, "Selected button ID out of range");
		return -EINVAL;
	}

	event.button_activity.button_pin = btn_cfg[btn_idx].btn_pin;
	event.button_activity.button_action = BUTTON_PRESS;
	event.event_source = EVT_SRC_BUTTON;

	ret = ctrl_events_put(&event);

	if (ret == -ENOMSG) {
		LOG_WRN("Event queue is full, ignoring button press");
		ret = 0;
	}

	ERR_CHK(ret);

	shell_print(shell, "Pushed button idx: %d pin: %d : %s", btn_idx, btn_cfg[btn_idx].btn_pin,
		    btn_cfg[btn_idx].btn_name);

	return 0;
}

/* Creating subcommands (level 1 command) array for command "demo". */
SHELL_STATIC_SUBCMD_SET_CREATE(buttons_cmd,
			       SHELL_COND_CMD(CONFIG_SHELL, print, NULL, "Print all buttons.",
					      cmd_print_all_btns),
			       SHELL_COND_CMD(CONFIG_SHELL, push, NULL, "Push button.",
					      cmd_push_btn),
			       SHELL_SUBCMD_SET_END);
/* Creating root (level 0) command "demo" without a handler */
SHELL_CMD_REGISTER(buttons, &buttons_cmd, "List and push buttons", NULL);
