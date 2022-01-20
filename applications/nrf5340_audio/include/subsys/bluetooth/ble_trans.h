/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
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

#ifndef _BLE_TRANS_H_
#define _BLE_TRANS_H_

#include <zephyr.h>
#include <bluetooth/conn.h>

/* Connection interval is calculated as x*1.25 */
#if (CONFIG_SW_CODEC_LC3_7_5_MS_FRAMESIZE && CONFIG_SW_CODEC_LC3)
/* Connection interval of 7.5 ms */
#define BLE_ISO_CONN_INTERVAL 6
#else
/* Connection interval of 10 ms */
#define BLE_ISO_CONN_INTERVAL 8
#endif /* (CONFIG_SW_CODEC_LC3_7_5_MS_FRAMESIZE && CONFIG_SW_CODEC_LC3) */

typedef enum {
	BLE_TRANS_CHANNEL_RETURN_MONO = 0,
	BLE_TRANS_CHANNEL_LEFT = 0,
	BLE_TRANS_CHANNEL_RIGHT,
	BLE_TRANS_CHANNEL_STEREO,
	BLE_TRANS_CHANNEL_NUM,
} ble_trans_chan_type_t;

typedef enum {
	TRANS_TYPE_NOT_SET, //< Default transport type. Not in use.
	TRANS_TYPE_BIS, //< Broadcast isochronous stream.
	TRANS_TYPE_CIS, //< Connected isochronous stream.
	TRANS_TYPE_NUM, //< Number of transport types.
} iso_trans_type_t;

typedef enum {
	DIR_NOT_SET,
	DIR_RX,
	DIR_TX,
	DIR_BIDIR,
	DIR_NUM,
} iso_direction_t;

/**@brief  BLE events
 */
typedef enum {
	BLE_EVT_CONNECTED,
	BLE_EVT_DISCONNECTED,
	BLE_EVT_LINK_READY,
	BLE_EVT_STREAMING,
	BLE_EVT_NUM_EVTS
} ble_evt_type_t;

/**@brief  BLE event structure
 */
typedef struct {
	ble_evt_type_t type;
} ble_evt_t;

/**@brief	BLE data callback type.
 *
 * @param data			Pointer to received data
 * @param size			Size of received data
 * @param bad_frame		Indicating if the frame is a bad frame or not
 * @param ts			ISO timestamp
 */
typedef void (*ble_trans_iso_rx_cb_t)(const uint8_t *const data, size_t size, bool bad_frame,
				      uint32_t ts);

/**@brief	Enable the ISO packet lost notify feature
 *
 * @return	0 for success, error otherwise.
 */
int ble_trans_iso_lost_notify_enable(void);

/**@brief	Send data over the ISO transport
 *		Could be either CIS or BIS dependent on configuration
 * @param data	Data to send
 * @param size	Size of data to send
 * @param chan_type Channel type (stereo or mono)
 *
 * @return	0 for success, error otherwise.
 */
int ble_trans_iso_tx(uint8_t const *const data, size_t size, ble_trans_chan_type_t chan_type);

/**@brief	Start iso stream
 *
 * @note	Type and direction is set by init
 *
 * @return	0 for success, error otherwise
 */
int ble_trans_iso_start(void);

/**@brief	Stop iso stream
 *
 * @note	Type and direction is set by init
 *
 * @return	0 for success, error otherwise
 */
int ble_trans_iso_stop(void);

/**
 * @brief    Trigger the scan for BIS
 *
 * @return	0 for success, error otherwise
 */
int ble_trans_iso_bis_rx_sync_get(void);

/**@brief Create ISO CIG
 *
 * @return 0 if successful, error otherwise
 */
int ble_trans_iso_cig_create(void);

/**@brief	Connect CIS ISO channel
 *
 * @param	conn ACL connection for CIS to connect
 *
 * @return	0 if successful, error otherwise
 */
int ble_trans_iso_cis_connect(struct bt_conn *conn);

/**@brief	Initialize either a CIS or BIS transport
 *
 * @return	0 for success, error otherwise
 */

int ble_trans_iso_init(iso_trans_type_t trans_type, iso_direction_t dir,
		       ble_trans_iso_rx_cb_t rx_cb);

#endif /* _BLE_TRANS_H_ */
