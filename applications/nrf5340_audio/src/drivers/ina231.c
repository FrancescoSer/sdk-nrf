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

#include "ina231.h"

#include <zephyr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <drivers/i2c.h>
#include <device.h>

#include "macros_common.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(ina231);

#define RESET_TIME_MS		   1
#define INA231_DEFAULT_CFG_REG_VAL (0x4127)

static const struct device *i2c_dev;
static ina231_config_t const *ina231_cfg;

static int reg_read(uint8_t addr, uint16_t *reg_val)
{
	int ret;
	struct i2c_msg msgs[2];
	uint16_t reg = 0;

	if (i2c_dev == NULL || ina231_cfg == NULL) {
		return -EIO;
	}

	memset(msgs, 0, sizeof(msgs));

	/* Setup I2C messages */
	/*Send the address to read from */
	msgs[0].buf = &addr;
	msgs[0].len = 1;
	msgs[0].flags = I2C_MSG_WRITE;

	/* Read from device. STOP after this. */
	msgs[1].buf = (uint8_t *)&reg;
	msgs[1].len = 2;
	msgs[1].flags = I2C_MSG_RESTART | I2C_MSG_READ | I2C_MSG_STOP;

	ret = i2c_transfer(i2c_dev, &msgs[0], 2, ina231_cfg->twi_addr);

	RET_IF_ERR(ret);

	*reg_val = (uint16_t)((reg & 0x00FF) << 8) | ((reg & 0xFF00) >> 8);

	return 0;
}

static int reg_write(uint8_t addr, uint16_t data)
{
	int ret;
	struct i2c_msg msg;
	uint8_t write_bytes[3];

	if (i2c_dev == NULL || ina231_cfg == NULL) {
		return -EIO;
	}

	memset(&msg, 0, sizeof(msg));

	write_bytes[0] = addr;
	write_bytes[1] = ((data & 0xFF00) >> 8);
	write_bytes[2] = (data & 0x00FF);

	/* Setup I2C message */
	msg.buf = write_bytes;
	msg.len = 3;
	msg.flags = I2C_MSG_WRITE | I2C_MSG_STOP;

	ret = i2c_transfer(i2c_dev, &msg, 1, ina231_cfg->twi_addr);
	RET_IF_ERR(ret);

	return 0;
}

int ina231_open(ina231_config_t const *cfg)
{
	if (ina231_cfg != NULL || cfg == NULL) {
		return -EINVAL;
	}

	ina231_cfg = cfg;

	if (i2c_dev == NULL) {
		i2c_dev = device_get_binding("I2C_1");

		if (i2c_dev == NULL) {
			return -EIO;
		}
	}

	return 0;
}

int ina231_close(ina231_config_t const *cfg)
{
	if (cfg == NULL || ina231_cfg != cfg || ina231_cfg == NULL) {
		return -EINVAL;
	}

	ina231_cfg = NULL;

	return 0;
}

int ina231_reset(void)
{
	int ret;

	ret = reg_write(REG_INA231_CONFIGURATION, INA231_CONFIG_RST_Msk);
	RET_IF_ERR(ret);

	/* Wait for device reset to complete. */
	(void)k_sleep(K_MSEC(RESET_TIME_MS));

	/* Check that reset bit is cleared and default values set in the config reg */
	uint16_t cfg_read;

	reg_read(REG_INA231_CONFIGURATION, &cfg_read);
	RET_IF_ERR(ret);

	if (cfg_read != INA231_DEFAULT_CFG_REG_VAL) {
		LOG_ERR("Cfg reg reads 0x%x Should be: 0x%x", cfg_read, INA231_DEFAULT_CFG_REG_VAL);
		return -EIO;
	}

	return ret;
}

int ina231_config_set(ina231_config_reg_t *config)
{
	uint16_t config_reg;

	config_reg = (config->avg << INA231_CONFIG_AVG_Pos) & INA231_CONFIG_AVG_Msk;
	config_reg |= (config->vbus_conv_time << INA231_CONFIG_VBUS_CONV_TIME_Pos) &
		      INA231_CONFIG_VBUS_CONV_TIME_Msk;
	config_reg |= (config->vsh_conv_time << INA231_CONFIG_VSH_CONV_TIME_Pos) &
		      INA231_CONFIG_VSH_CONV_TIME_Msk;
	config_reg |= (config->mode << INA231_CONFIG_MODE_Pos) & INA231_CONFIG_MODE_Msk;
	/* Bit 14 needs to be set according to datasheet. */
	config_reg |= (INA231_CONFIG_MAGIC_ONE_Default << INA231_CONFIG_MAGIC_ONE_Pos) &
		      INA231_CONFIG_MAGIC_ONE_Msk;

	return reg_write(REG_INA231_CONFIGURATION, config_reg);
}

int ina231_config_get(ina231_config_reg_t *config)
{
	int ret;

	uint16_t config_reg;

	ret = reg_read(REG_INA231_CONFIGURATION, &config_reg);
	RET_IF_ERR(ret);

	config->avg = (ina231_config_avg_t)((config_reg & INA231_CONFIG_AVG_Msk) >>
					    INA231_CONFIG_AVG_Pos);

	config->vbus_conv_time =
		(ina231_config_vbus_conv_time_t)((config_reg & INA231_CONFIG_VBUS_CONV_TIME_Msk) >>
						 INA231_CONFIG_VBUS_CONV_TIME_Pos);

	config->vsh_conv_time = (ina231_config_vsh_conv_time_t)(
		(config_reg & INA231_CONFIG_VSH_CONV_TIME_Msk) >> INA231_CONFIG_VSH_CONV_TIME_Pos);

	config->mode = (ina231_config_mode_t)((config_reg & INA231_CONFIG_MODE_Msk) >>
					      INA231_CONFIG_MODE_Pos);

	return 0;
}

int ina231_shunt_voltage_reg_get(uint16_t *p_voltage)
{
	return reg_read(REG_INA231_SHUNT_VOLTAGE, p_voltage);
}

int ina231_bus_voltage_reg_get(uint16_t *p_voltage)
{
	return reg_read(REG_INA231_BUS_VOLTAGE, p_voltage);
}

int ina231_power_reg_get(uint16_t *p_power)
{
	return reg_read(REG_INA231_POWER, p_power);
}

int ina231_current_reg_get(uint16_t *p_current)
{
	return reg_read(REG_INA231_CURRENT, p_current);
}

int ina231_calibration_set(uint16_t calib)
{
	return reg_write(REG_INA231_CALIBRATION, calib);
}

int ina231_calibration_get(uint16_t *p_calib)
{
	return reg_read(REG_INA231_CALIBRATION, p_calib);
}

int ina231_mask_enable_set(uint16_t mask_enable)
{
	return reg_write(REG_INA231_MASK_ENABLE, mask_enable);
}

int ina231_mask_enable_get(uint16_t *p_mask_enable)
{
	return reg_read(REG_INA231_MASK_ENABLE, p_mask_enable);
}

int ina231_alert_limit_set(uint16_t alert_limit)
{
	return reg_write(REG_INA231_MASK_ENABLE, alert_limit);
}

int ina231_alert_limit_get(uint16_t *p_alert_limit)
{
	return reg_read(REG_INA231_MASK_ENABLE, p_alert_limit);
}

float conv_time_enum_to_sec(ina231_config_vbus_conv_time_t conv_time)
{
	switch (conv_time) {
	case INA231_CONFIG_VBUS_CONV_TIME_140_US:
		return 0.00014;
	case INA231_CONFIG_VBUS_CONV_TIME_204_US:
		return 0.000204;
	case INA231_CONFIG_VBUS_CONV_TIME_332_US:
		return 0.000332;
	case INA231_CONFIG_VBUS_CONV_TIME_588_US:
		return 0.000588;
	case INA231_CONFIG_VBUS_CONV_TIME_1100_US:
		return 0.0011;
	case INA231_CONFIG_VBUS_CONV_TIME_2116_US:
		return 0.002116;
	case INA231_CONFIG_VBUS_CONV_TIME_4156_US:
		return 0.004156;
	case INA231_CONFIG_VBUS_CONV_TIME_8244_US:
		return 0.008244;
	default:
		LOG_ERR("Invalid conversion time: %d", conv_time);
		ERR_CHK(-EINVAL);
	}

	CODE_UNREACHABLE;
	return 0;
}

uint16_t average_enum_to_int(ina231_config_avg_t average)
{
	switch (average) {
	case INA231_CONFIG_AVG_1:
		return 1;
	case INA231_CONFIG_AVG_4:
		return 4;
	case INA231_CONFIG_AVG_16:
		return 16;
	case INA231_CONFIG_AVG_64:
		return 64;
	case INA231_CONFIG_AVG_128:
		return 128;
	case INA231_CONFIG_AVG_256:
		return 256;
	case INA231_CONFIG_AVG_512:
		return 512;
	case INA231_CONFIG_AVG_1024:
		return 1024;
	default:
		LOG_ERR("Invalid average: %d", average);
		ERR_CHK(-EINVAL);
	}

	CODE_UNREACHABLE;
	return 0;
}
