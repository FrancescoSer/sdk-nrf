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

#ifndef _BLE_ACL_COMMON_H_
#define _BLE_ACL_COMMON_H_

#include "ble_trans.h"
#include <bluetooth/conn.h>

#define DEVICE_NAME_PEER_L     CONFIG_BLE_DEVICE_NAME_BASE "_H_L"
#define DEVICE_NAME_PEER_L_LEN (sizeof(DEVICE_NAME_PEER_L) - 1)

#define DEVICE_NAME_PEER_R     CONFIG_BLE_DEVICE_NAME_BASE "_H_R"
#define DEVICE_NAME_PEER_R_LEN (sizeof(DEVICE_NAME_PEER_R) - 1)

/**@brief Set pointer to peer connection in ble_acl_common.c
 *
 * @param conn	Connection to peer
 */
void ble_acl_common_conn_peer_set(struct bt_conn *conn);

/**@brief Get pointer from the peer connection
 *
 * @param p_conn Pointer for peer connection information
 */
void ble_acl_common_conn_peer_get(struct bt_conn **p_conn);

/**@brief Start ACL GAP event, scan on gateway and advertising on headset
 */
void ble_acl_common_start(void);

/**@brief Register callback functions for ACL, and also service init
 *
 * @return 0 if successful, error otherwise
 */
int ble_acl_common_init(void);

#endif /* _BLE_ACL_COMMON_H_ */
