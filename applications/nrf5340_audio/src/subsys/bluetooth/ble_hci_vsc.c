/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <sys/byteorder.h>
#include <drivers/gpio.h>
#include "macros_common.h"
#include "ble_hci_vsc.h"
#include "bluetooth/hci.h"

#include <logging/log.h>
LOG_MODULE_DECLARE(ble, CONFIG_LOG_BLE_LEVEL);

/* Enable VREGRADIO.VREQH in NET core for getting +3dBm */
static int ble_hci_vsc_radio_pwr_cfg(bool high_power_mode)
{
	int ret;
	struct ble_hci_vs_cp_set_radio_fe_cfg *cp;
	struct ble_hci_vs_rp_status *rp;
	struct net_buf *buf, *rsp = NULL;

	buf = bt_hci_cmd_create(HCI_OPCODE_VS_SET_RADIO_FE_CFG, sizeof(*cp));
	if (!buf) {
		LOG_ERR("Unable to allocate command buffer");
		return -ENOMEM;
	}
	cp = net_buf_add(buf, sizeof(*cp));
	if (high_power_mode) {
		LOG_DBG("Enable VREGRADIO.VREQH");
		cp->max_tx_power = BLE_HCI_VSC_TX_PWR_Pos3dBm;
	} else {
		LOG_DBG("Disable VREGRADIO.VREQH");
		cp->max_tx_power = BLE_HCI_VSC_TX_PWR_0dBm;
	}
	cp->ant_id = 0;

	ret = bt_hci_cmd_send_sync(HCI_OPCODE_VS_SET_RADIO_FE_CFG, buf, &rsp);
	RET_IF_ERR_MSG(ret, "Error for HCI VS command HCI_OPCODE_VS_SET_ADV_TX_PWR");

	rp = (void *)rsp->data;
	ret = rp->status;
	net_buf_unref(rsp);
	return ret;
}

int ble_hci_vsc_set_bd_addr(uint8_t *bd_addr)
{
	int ret;
	struct ble_hci_vs_cp_set_bd_addr *cp;
	struct net_buf *buf = NULL;

	buf = bt_hci_cmd_create(HCI_OPCODE_VS_SET_BD_ADDR, sizeof(*cp));
	if (!buf) {
		LOG_ERR("Unable to allocate command buffer");
		return -ENOMEM;
	}
	cp = net_buf_add(buf, sizeof(*cp));
	memcpy(cp, bd_addr, sizeof(*cp));

	ret = bt_hci_cmd_send(HCI_OPCODE_VS_SET_BD_ADDR, buf);
	return ret;
}

int ble_hci_vsc_set_op_flag(uint32_t flag_bit, uint8_t setting)
{
	int ret;
	struct ble_hci_vs_cp_set_op_flag *cp;
	struct ble_hci_vs_rp_status *rp;
	struct net_buf *buf, *rsp = NULL;

	buf = bt_hci_cmd_create(HCI_OPCODE_VS_SET_OP_FLAGS, sizeof(*cp));
	if (!buf) {
		LOG_ERR("Unable to allocate command buffer");
		return -ENOMEM;
	}
	cp = net_buf_add(buf, sizeof(*cp));
	cp->flag_bit = flag_bit;
	cp->setting = setting;
	ret = bt_hci_cmd_send_sync(HCI_OPCODE_VS_SET_OP_FLAGS, buf, &rsp);
	RET_IF_ERR_MSG(ret, "Error for HCI VS command HCI_OPCODE_VS_SET_OP_FLAGS");

	rp = (void *)rsp->data;
	ret = rp->status;
	net_buf_unref(rsp);
	return ret;
}

int ble_hci_vsc_set_adv_tx_pwr(ble_hci_vs_tx_power_t tx_power)
{
	int ret;
	struct ble_hci_vs_cp_set_adv_tx_pwr *cp;
	struct ble_hci_vs_rp_status *rp;
	struct net_buf *buf, *rsp = NULL;

	if (tx_power == BLE_HCI_VSC_TX_PWR_Pos3dBm) {
		ret = ble_hci_vsc_radio_pwr_cfg(true);
	} else {
		ret = ble_hci_vsc_radio_pwr_cfg(false);
	}
	RET_IF_ERR_MSG(ret, "Failed to set maximum TX power when set ADV TX power");

	buf = bt_hci_cmd_create(HCI_OPCODE_VS_SET_ADV_TX_PWR, sizeof(*cp));
	if (!buf) {
		LOG_ERR("Unable to allocate command buffer");
		return -ENOMEM;
	}
	cp = net_buf_add(buf, sizeof(*cp));
	cp->tx_power = tx_power;

	ret = bt_hci_cmd_send_sync(HCI_OPCODE_VS_SET_ADV_TX_PWR, buf, &rsp);
	RET_IF_ERR_MSG(ret, "Error for HCI VS command HCI_OPCODE_VS_SET_ADV_TX_PWR");

	rp = (void *)rsp->data;
	ret = rp->status;
	net_buf_unref(rsp);
	return ret;
}

int ble_hci_vsc_set_conn_tx_pwr(uint16_t conn_handle, ble_hci_vs_tx_power_t tx_power)
{
	int ret;
	struct ble_hci_vs_cp_set_conn_tx_pwr *cp;
	struct ble_hci_vs_rp_status *rp;
	struct net_buf *buf, *rsp = NULL;

	if (tx_power == BLE_HCI_VSC_TX_PWR_Pos3dBm) {
		ret = ble_hci_vsc_radio_pwr_cfg(true);
	} else {
		ret = ble_hci_vsc_radio_pwr_cfg(false);
	}
	RET_IF_ERR_MSG(ret, "Failed to set maximum TX power when set CONN TX power");

	buf = bt_hci_cmd_create(HCI_OPCODE_VS_SET_CONN_TX_PWR, sizeof(*cp));
	if (!buf) {
		LOG_ERR("Unable to allocate command buffer");
		return -ENOMEM;
	}
	cp = net_buf_add(buf, sizeof(*cp));
	cp->handle = conn_handle;
	cp->tx_power = tx_power;

	ret = bt_hci_cmd_send_sync(HCI_OPCODE_VS_SET_CONN_TX_PWR, buf, &rsp);
	RET_IF_ERR_MSG(ret, "Error for HCI VS command HCI_OPCODE_VS_SET_CONN_TX_PWR");

	rp = (void *)rsp->data;
	ret = rp->status;
	net_buf_unref(rsp);
	return ret;
}

int ble_hci_vsc_map_led_pin(ble_hci_vs_led_function_id_t id, ble_hci_vs_led_function_mode_t mode,
			    uint16_t pin)
{
	int ret;
	struct ble_hci_vs_cp_set_led_pin_map *cp;
	struct ble_hci_vs_rp_status *rp;
	struct net_buf *buf, *rsp = NULL;

	buf = bt_hci_cmd_create(HCI_OPCODE_VS_SET_LED_PIN_MAP, sizeof(*cp));
	if (!buf) {
		LOG_ERR("Unable to allocate command buffer");
		return -ENOMEM;
	}

	cp = net_buf_add(buf, sizeof(*cp));
	cp->id = id;

	if (mode == GPIO_ACTIVE_LOW) {
		cp->mode = PAL_LED_MODE_ACTIVE_LOW;
	} else {
		cp->mode = PAL_LED_MODE_ACTIVE_HIGH;
	}

	cp->pin = pin;

	ret = bt_hci_cmd_send_sync(HCI_OPCODE_VS_SET_LED_PIN_MAP, buf, &rsp);
	RET_IF_ERR_MSG(ret, "Error for HCI VS command HCI_OPCODE_VS_SET_LED_PIN_MAP");

	rp = (void *)rsp->data;
	ret = rp->status;
	net_buf_unref(rsp);
	return ret;
}
