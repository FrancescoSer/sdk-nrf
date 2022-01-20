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

#include <zephyr.h>
#include <debug/stack.h>
#include <device.h>
#include <string.h>
#include <drivers/gpio.h>

#include "macros_common.h"
#include "fw_info_app.h"
#include "led.h"
#include "button_handler.h"
#include "button_assignments.h"
#include "audio_codec.h"
#include "nrfx_clock.h"
#include "nrfx_gpiote.h"
#include "streamctrl.h"
#include "ble_core.h"
#include "pmic.h"
#include "board.h"
#include "power_module.h"
#include "sd_card.h"
#include "board_version.h"
#include "devicetree.h"
#include "audio_datapath.h"
#include "audio_i2s.h"
#include "audio_sync_timer.h"
#include "channel_assignment.h"
#include "hw_codec.h"
#include "audio_usb.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_LOG_MAIN_LEVEL);

#if defined(CONFIG_INIT_STACKS)
/* Used for printing stack usage */
extern struct k_thread z_main_thread;
#endif /* defined(CONFIG_INIT_STACKS) */

static atomic_t ble_core_is_ready = (atomic_t) false;
static board_version_t board_rev;
#define SHARED_SPI DT_NODELABEL(spi4) /* SD card and HW codec share the SPI4 */

static int core_app_config(void)
{
	int ret;

	nrf_gpiote_latency_t latency = nrfx_gpiote_latency_get();

	if (latency != NRF_GPIOTE_LATENCY_LOWPOWER) {
		LOG_DBG("Setting gpiote latency to low power");
		nrfx_gpiote_latency_set(NRF_GPIOTE_LATENCY_LOWPOWER);
	}

	/* Use this to turn on 128 MHz clock for cpu_app */
	ret = nrfx_clock_divider_set(NRF_CLOCK_DOMAIN_HFCLK, NRF_CLOCK_HFCLK_DIV_1);
	RET_IF_ERR(ret - NRFX_ERROR_BASE_NUM);

	nrfx_clock_hfclk_start();
	while (!nrfx_clock_hfclk_is_running()) {
	}

	/* Workaround for issue with PCA10121 v0.7.0 related to SD-card */
	static const struct device *gpio_dev;

	gpio_dev = device_get_binding(DT_SPI_DEV_CS_GPIOS_LABEL(DT_NODELABEL(sdhc0)));
	if (!gpio_dev) {
		return -ENODEV;
	}

	ret = gpio_pin_configure(gpio_dev, DT_PROP(SHARED_SPI, mosi_pin),
				 GPIO_DS_ALT_HIGH | GPIO_DS_ALT_LOW);
	RET_IF_ERR(ret);
	ret = gpio_pin_configure(gpio_dev, DT_PROP(SHARED_SPI, sck_pin),
				 GPIO_DS_ALT_HIGH | GPIO_DS_ALT_LOW);
	RET_IF_ERR(ret);

	return 0;
}

static int leds_set(void)
{
	int ret;

	/* Blink LED 3 to indicate that APP core is running */
	ret = led_blink(LED_APP_3_GREEN);
	RET_IF_ERR(ret);

#if (NRF5340_AUDIO_DEV == NRF5340_AUDIO_DEV_HEADSET)
	enum audio_channel channel;

	ret = channel_assignment_get(&channel);
	if (ret) {
		/* Channel is not assigned yet: use default */
		channel = AUDIO_CHANNEL_DEFAULT;
	}

	if (channel == AUDIO_CHANNEL_LEFT) {
		ret = led_on(LED_APP_RGB, LED_COLOR_BLUE);
	} else {
		ret = led_on(LED_APP_RGB, LED_COLOR_MAGENTA);
	}

	RET_IF_ERR(ret);
#elif NRF5340_AUDIO_DEV == NRF5340_AUDIO_DEV_GATEWAY
	ret = led_on(LED_APP_RGB, LED_COLOR_GREEN);
	RET_IF_ERR(ret);
#endif /* (NRF5340_AUDIO_DEV == NRF5340_AUDIO_DEV_HEADSET) */

	return 0;
}

static int channel_assign_check(void)
{
#if (NRF5340_AUDIO_DEV == NRF5340_AUDIO_DEV_HEADSET) && CONFIG_AUDIO_HEADSET_CHANNEL_RUNTIME
	if (!channel_assignment_get(&(enum audio_channel){ AUDIO_CHANNEL_LEFT })) {
		/* Channel already assigned */
		return 0;
	}

	int ret;
	bool pressed;

	ret = button_pressed(BUTTON_VOLUME_DOWN, &pressed);
	RET_IF_ERR(ret);

	if (pressed) {
		return channel_assignment_set(AUDIO_CHANNEL_LEFT);
	}

	ret = button_pressed(BUTTON_VOLUME_UP, &pressed);
	RET_IF_ERR(ret);

	if (pressed) {
		return channel_assignment_set(AUDIO_CHANNEL_RIGHT);
	}
#endif /* (NRF5340_AUDIO_DEV == NRF5340_AUDIO_DEV_HEADSET) && CONFIG_AUDIO_HEADSET_CHANNEL_RUNTIME */

	return 0;
}

/* Callback from ble_core when the ble subsystem is ready */
void on_ble_core_ready(void)
{
	(void)atomic_set(&ble_core_is_ready, (atomic_t) true);
}

void main(void)
{
	int ret;

	LOG_DBG("nRF5340 APP core started");

	ret = led_init();
	ERR_CHK(ret);

	ret = button_handler_init();
	ERR_CHK(ret);

	ret = channel_assign_check();
	ERR_CHK(ret);

	ret = fw_info_app_print();
	ERR_CHK(ret);

	ret = core_app_config();
	ERR_CHK(ret);

	ret = board_version_valid_check();
	ERR_CHK(ret);

	ret = board_version_get(&board_rev);
	ERR_CHK(ret);

	if (board_rev.mask & BOARD_REVISION_VALID_MSK_MAX14690_PMIC) {
		ret = pmic_init();
		ERR_CHK(ret);

		ret = pmic_defaults_set();
		ERR_CHK(ret);
	}

	if (board_rev.mask & BOARD_VERSION_VALID_MSK_SD_CARD) {
		ret = sd_card_init();
		if (ret != -ENODEV) {
			ERR_CHK(ret);
		}
	}

	ret = power_module_init();
	ERR_CHK(ret);

#if ((NRF5340_AUDIO_DEV == NRF5340_AUDIO_DEV_GATEWAY) && (CONFIG_AUDIO_SOURCE_USB))
	ret = audio_usb_init();
	ERR_CHK(ret);
#else
	ret = audio_datapath_init();
	ERR_CHK(ret);
	ret = audio_sync_timer_init();
	ERR_CHK(ret);
	audio_i2s_init();
	ret = hw_codec_init();
	ERR_CHK(ret);
	audio_sync_timer_sync_evt_send();
#endif /* ((NRF5340_AUDIO_DEV == NRF5340_AUDIO_DEV_GATEWAY) && (CONFIG_AUDIO_SOURCE_USB)) */

	/* Initialize BLE, with callback for when BLE is ready */
	ret = ble_core_init(on_ble_core_ready);
	ERR_CHK(ret);

	/* Wait until ble_core/NET core is ready */
	while (!(bool)atomic_get(&ble_core_is_ready)) {
		(void)k_sleep(K_MSEC(100));
	}

	ret = streamctrl_start();
	ERR_CHK(ret);

	ret = leds_set();
	ERR_CHK(ret);

	while (1) {
		streamctrl_event_handler();
		STACK_USAGE_PRINT("main", &z_main_thread);
	}
}
