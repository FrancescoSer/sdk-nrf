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

#ifndef _BLE_AUDIO_SERVICES_H_
#define _BLE_AUDIO_SERVICES_H_

/**
 * @brief Discover VCS and included services
 *
 * @param conn Pointer for peer connection information
 * @param channel_num The number of the remote device
 *
 * This will start a GATT discovery and setup handles and subscriptions for
 * VCS and included services.
 * This shall be called once before any other actions related with VCS.
 *
 * @return 0 for success, error otherwise.
 */
int ble_vcs_discover(struct bt_conn *conn, uint8_t channel_num);

/**
 * @brief Initialize the Volume Control Service client
 */
int ble_vcs_client_init(void);

/**
 * @brief  Set volume to a specific value.
 * @param  volume The absolute volume to set.
 *         If the current device is gateway, the target device
 *         will be headset, if the current device is headset then
 *         the target device will be itself.
 *
 * @return 0 for success,
 *          -ENXIO if the feature is disabled
 *          Other errors from underlying drivers.
 */
int ble_vcs_vol_set(uint8_t volume);

/**
 * @brief  Turn the volume up by one step.
 *
 *         If the current device is gateway, the target device
 *         will be headset, if the current device is headset then
 *         the target device will be itself.
 *
 * @return 0 for success,
 *         -ENXIO if the feature is disabled
 *         Other errors from underlying drivers.
 */
int ble_vcs_volume_up(void);

/**
 * @brief  Turn the volume down by one step.
 *
 *         If the current device is gateway, the target device
 *         will be headset, if the current device is headset then
 *         the target device will be itself.
 *
 * @return 0 for success,
 *         -ENXIO if the feature is disabled
 *         Other errors from underlying drivers.
 */
int ble_vcs_volume_down(void);

/**
 * @brief  Mute the output volume of the device.
 *
 *         If the current device is gateway, the target device
 *         will be headset, if the current device is headset then
 *         the target device will be itself.
 *
 * @return 0 for success,
 *         -ENXIO if the feature is disabled
 *         Other errors from underlying drivers.
 */
int ble_vcs_volume_mute(void);

/**
 * @brief  Unmute the output volume of the device.
 *
 *         If the current device is gateway, the target device
 *         will be headset, if the current device is headset then
 *         the target device will be itself.
 *
 * @return 0 for success,
 *         -ENXIO if the feature is disabled
 *         Other errors from underlying drivers.
 */
int ble_vcs_volume_unmute(void);

/**
 * @brief Initialize the Volume Control Service server
 *
 * @return 0 for success, error otherwise.
 */
int ble_vcs_server_init(void);

#endif /* _BLE_AUDIO_SERVICES_H_ */
