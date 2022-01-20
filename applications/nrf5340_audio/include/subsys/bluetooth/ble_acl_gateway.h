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

#ifndef _BLE_ACL_GATEWAY_H_
#define _BLE_ACL_GATEWAY_H_

#include <bluetooth/conn.h>
#include <kernel.h>

/**@brief Get pointer from the peer connection
 *
 * @param[in]	chan_num	The channel of the connection handle to get
 * @param[out]	p_conn		Pointer for peer connection information
 *
 * @return 0 for success, -EINVAL for channel number out of index
 */
int ble_acl_gateway_conn_peer_get(uint8_t chan_num, struct bt_conn **p_conn);

/**@brief Set pointer for the peer connection
 *
 * @param[in]	chan_num	The channel of the connection handle to set
 * @param[out]	p_conn		Pointer for peer connection information
 *
 * @return 0 for success, -EINVAL for channel number out of index
 */
int ble_acl_gateway_conn_peer_set(uint8_t chan_num, struct bt_conn **p_conn);

/**@brief Check if gateway is connected to all headsets over ACL link
 *
 * @return true if all ACL links connected, false otherwise
 */
bool ble_acl_gateway_all_links_connected(void);

/**@brief Work handler for scanning for peer connection
 *
 * @param item The work item that provided the handler
 */
void work_scan_start(struct k_work *item);

/**@brief BLE gateway connected handler
 *
 * @param conn	Connection to peer
 */
void ble_acl_gateway_on_connected(struct bt_conn *conn);

/**@brief Start the MTU exchange procedure.
 *
 * @param conn	Connection to peer
 *
 * @return 0 for success, error otherwise.
 */
int ble_acl_gateway_mtu_exchange(struct bt_conn *conn);

#endif /* _BLE_ACL_GATEWAY_H_ */
