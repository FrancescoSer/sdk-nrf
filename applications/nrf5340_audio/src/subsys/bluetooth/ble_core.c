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

#include "ble_core.h"
#include "ble_hci_vsc.h"
#include <zephyr.h>
#include <bluetooth/bluetooth.h>
#include <errno.h>
#include "macros_common.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(ble, CONFIG_LOG_BLE_LEVEL);

#define NETCORE_RESPONSE_TIMEOUT_MS 500

static ble_core_ready_t m_ready_callback;
static struct bt_le_oob _oob = { .addr = 0 };

static void netcore_timeout_handler(struct k_timer *timer_id);
K_TIMER_DEFINE(netcore_timeout_alarm_timer, netcore_timeout_handler, NULL);

/* If NET core out of response for a time defined in NET_CORE_RESPONSE_TIMEOUT
 * show error message for indicating user.
 */
static void netcore_timeout_handler(struct k_timer *timer_id)
{
	ERR_CHK_MSG(-EIO, "No response from NET core, check if NET core is programmed");
}

static void mac_print(void)
{
	char dev[BT_ADDR_LE_STR_LEN];
	(void)bt_le_oob_get_local(BT_ID_DEFAULT, &_oob);
	(void)bt_addr_le_to_str(&_oob.addr, dev, BT_ADDR_LE_STR_LEN);
	LOG_INF("MAC: %s\n", dev);
}

/* Callback called by the Bluetooth stack in Zephyr when Bluetooth is ready */
static void on_bt_ready(int err)
{
	if (err) {
		LOG_ERR("Bluetooth init failed (err %d)", err);
		ERR_CHK(err);
	}

	LOG_DBG("Bluetooth initialized");
	mac_print();
	m_ready_callback();
}

static int controller_leds_mapping(void)
{
	int ret;

	ret = ble_hci_vsc_map_led_pin(PAL_LED_ID_CPU_ACTIVE,
				      DT_GPIO_FLAGS_BY_IDX(DT_NODELABEL(rgb2_green), gpios, 0),
				      DT_GPIO_PIN_BY_IDX(DT_NODELABEL(rgb2_green), gpios, 0));
	RET_IF_ERR(ret);

	return 0;
}

int ble_core_le_pwr_ctrl_disable(void)
{
	return ble_hci_vsc_set_op_flag(BLE_HCI_VSC_OP_DIS_POWER_MONITOR, 1);
}

int ble_core_init(ble_core_ready_t ready_callback)
{
	int ret;

	if (ready_callback == NULL) {
		return -EINVAL;
	}
	m_ready_callback = ready_callback;

	/* Setup a timer for monitoring if NET core is working or not */
	k_timer_start(&netcore_timeout_alarm_timer, K_MSEC(NETCORE_RESPONSE_TIMEOUT_MS), K_NO_WAIT);

	/* Enable Bluetooth, with callback function that
	 * will be called when Bluetooth is ready
	 */
	ret = bt_enable(on_bt_ready);
	k_timer_stop(&netcore_timeout_alarm_timer);
	if (ret) {
		LOG_ERR("Bluetooth init failed (ret %d)", ret);
		return ret;
	}

	ret = controller_leds_mapping();
	if (ret) {
		LOG_ERR("Error mapping LED pins to the Bluetooth controller (ret %d)", ret);
		return ret;
	}

	return 0;
}
