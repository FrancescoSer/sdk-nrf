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

#ifndef _BLE_HCI_VSC_H_
#define _BLE_HCI_VSC_H_

#include <zephyr.h>

#define HCI_OPCODE_VS_SET_OP_FLAGS     BT_OP(BT_OGF_VS, 0x3F3)
#define HCI_OPCODE_VS_SET_BD_ADDR      BT_OP(BT_OGF_VS, 0x3F0)
#define HCI_OPCODE_VS_SET_ADV_TX_PWR   BT_OP(BT_OGF_VS, 0x3F5)
#define HCI_OPCODE_VS_SET_CONN_TX_PWR  BT_OP(BT_OGF_VS, 0x3F6)
#define HCI_OPCODE_VS_SET_LED_PIN_MAP  BT_OP(BT_OGF_VS, 0x3A2)
#define HCI_OPCODE_VS_SET_RADIO_FE_CFG BT_OP(BT_OGF_VS, 0x3A3)

/* This bit setting enables the flag from controller from controller
 * if an ISO packet is lost.
 */
#define BLE_HCI_VSC_OP_ISO_LOST_NOTIFY	 (1 << 17)
#define BLE_HCI_VSC_OP_DIS_POWER_MONITOR (1 << 15)

struct ble_hci_vs_rp_status {
	int8_t status;
} __packed;

struct ble_hci_vs_cp_set_op_flag {
	uint32_t flag_bit;
	uint8_t setting;
} __packed;

struct ble_hci_vs_cp_set_bd_addr {
	uint8_t bd_addr[6];
} __packed;

struct ble_hci_vs_cp_set_adv_tx_pwr {
	int8_t tx_power;
} __packed;

struct ble_hci_vs_cp_set_conn_tx_pwr {
	uint16_t handle;
	int8_t tx_power;
} __packed;

struct ble_hci_vs_cp_set_led_pin_map {
	uint8_t id;
	uint8_t mode;
	uint16_t pin;
} __packed;

struct ble_hci_vs_cp_set_radio_fe_cfg {
	int8_t max_tx_power;
	uint8_t ant_id;
} __packed;

typedef enum {
	BLE_HCI_VSC_TX_PWR_Pos3dBm = 3,
	BLE_HCI_VSC_TX_PWR_0dBm = 0,
	BLE_HCI_VSC_TX_PWR_Neg1dBm = -1,
	BLE_HCI_VSC_TX_PWR_Neg2dBm = -2,
	BLE_HCI_VSC_TX_PWR_Neg3dBm = -3,
	BLE_HCI_VSC_TX_PWR_Neg4dBm = -4,
	BLE_HCI_VSC_TX_PWR_Neg5dBm = -5,
	BLE_HCI_VSC_TX_PWR_Neg6dBm = -6,
	BLE_HCI_VSC_TX_PWR_Neg7dBm = -7,
	BLE_HCI_VSC_TX_PWR_Neg8dBm = -8,
	BLE_HCI_VSC_TX_PWR_Neg12dBm = -12,
	BLE_HCI_VSC_TX_PWR_Neg16dBm = -16,
	BLE_HCI_VSC_TX_PWR_Neg20dBm = -20,
	BLE_HCI_VSC_TX_PWR_Neg40dBm = -40,
} ble_hci_vs_tx_power_t;

typedef enum {
	PAL_LED_ID_CPU_ACTIVE = 0x10,
	PAL_LED_ID_ERROR = 0x11,
	PAL_LED_ID_BLE_TX = 0x12,
	PAL_LED_ID_BLE_RX = 0x13,
} ble_hci_vs_led_function_id_t;

typedef enum {
	PAL_LED_MODE_ACTIVE_LOW = 0x00,
	PAL_LED_MODE_ACTIVE_HIGH = 0x01,
	PAL_LED_MODE_DISABLE_TOGGLE = 0xFF,
} ble_hci_vs_led_function_mode_t;

/**
 * @brief Set Bluetooth MAC device address
 * @param bd_addr	Bluetooth MAC device address
 *
 * @return 0 for success, error otherwise.
 */
int ble_hci_vsc_set_bd_addr(uint8_t *bd_addr);

/**
 * @brief Set controller operation mode flag
 * @param flag_bit	The target bit in operation mode flag
 * @param setting	The setting of the bit
 *
 * @return 0 for success, error otherwise.
 */
int ble_hci_vsc_set_op_flag(uint32_t flag_bit, uint8_t setting);

/**
 * @brief Set advertising TX power
 * @param tx_power TX power setting for the advertising.
 *                 Please check ble_hci_vs_tx_power_t for possible settings
 *
 * @return 0 for success, error otherwise.
 */
int ble_hci_vsc_set_adv_tx_pwr(ble_hci_vs_tx_power_t tx_power);

/**
 * @brief Set TX power for specific connection
 * @param conn_handle Specific connection handle for TX power setting
 * @param tx_power TX power setting for the specific connection handle
 *                 Please check ble_hci_vs_tx_power_t for possible settings
 *
 * @return 0 for success, error otherwise.
 */
int ble_hci_vsc_set_conn_tx_pwr(uint16_t conn_handle, ble_hci_vs_tx_power_t tx_power);

/**
 * @brief Map LED pin to a specific controller function
 *
 * @details Only support for gpio0 (pin 0-31)
 *
 * @param id Describes the LED function
 *           Please check ble_hci_vs_led_function_id_t for possible IDs
 * @param mode Describes how the pin is toggled
 *           Please check ble_hci_vs_led_function_mode_t for possible modes
 * @param pin Pin designator of the GPIO
 *
 * @return 0 for success, error otherwise.
 */
int ble_hci_vsc_map_led_pin(ble_hci_vs_led_function_id_t id, ble_hci_vs_led_function_mode_t mode,
			    uint16_t pin);

#endif /* _BLE_HCI_VSC_H_ */
