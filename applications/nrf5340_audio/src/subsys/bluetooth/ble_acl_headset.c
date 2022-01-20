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

#include "ble_acl_headset.h"

#include <zephyr.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>

#include "macros_common.h"
#include "board.h"
#include "ble_acl_common.h"
#include "channel_assignment.h"

#include <logging/log.h>
LOG_MODULE_DECLARE(ble, CONFIG_LOG_BLE_LEVEL);

#define BT_LE_ADV_FAST_CONN \
	BT_LE_ADV_PARAM(BT_LE_ADV_OPT_CONNECTABLE, BT_GAP_ADV_FAST_INT_MIN_1, \
			BT_GAP_ADV_FAST_INT_MAX_1, NULL)

/* Advertising data for peer connection */
static const struct bt_data ad_peer_l[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME_PEER_L, DEVICE_NAME_PEER_L_LEN),
};

static const struct bt_data ad_peer_r[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME_PEER_R, DEVICE_NAME_PEER_R_LEN),
};

/* Connection to the gateway device - the other nRF5340 Audio device
 * This is the device we are streaming audio to/from.
 */
static struct bt_conn *headset_conn_peer;

void work_adv_start(struct k_work *item)
{
	enum audio_channel channel;
	int ret;

	ret = channel_assignment_get(&channel);
	if (ret) {
		/* Channel is not assigned yet: use default */
		channel = AUDIO_CHANNEL_DEFAULT;
	}

	if (channel != AUDIO_CHANNEL_RIGHT) {
		/* If anything else than right, default to left */
		ret = bt_le_adv_start(BT_LE_ADV_FAST_CONN, ad_peer_l, ARRAY_SIZE(ad_peer_l), NULL,
				      0);
	} else {
		ret = bt_le_adv_start(BT_LE_ADV_FAST_CONN, ad_peer_r, ARRAY_SIZE(ad_peer_r), NULL,
				      0);
	}

	if (ret) {
		LOG_ERR("Advertising failed to start (ret %d)", ret);
	}
}

void ble_acl_headset_on_connected(struct bt_conn *conn)
{
	LOG_DBG("Connected - nRF5340 Audio headset");
	headset_conn_peer = bt_conn_ref(conn);
	ble_acl_common_conn_peer_set(headset_conn_peer);
}
