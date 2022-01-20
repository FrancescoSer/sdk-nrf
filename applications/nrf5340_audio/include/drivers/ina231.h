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

#ifndef _INA231_H_
#define _INA231_H_

#include <stdint.h>

#define REG_INA231_CONFIGURATION 0x00
#define REG_INA231_SHUNT_VOLTAGE 0x01
#define REG_INA231_BUS_VOLTAGE	 0x02
#define REG_INA231_POWER	 0x03
#define REG_INA231_CURRENT	 0x04
#define REG_INA231_CALIBRATION	 0x05
#define REG_INA231_MASK_ENABLE	 0x06
#define REG_INA231_ALERT_LIMIT	 0x07

typedef struct {
	uint8_t twi_addr;
} ina231_config_t;

#define INA231_CONFIG_MODE_Pos (0)
#define INA231_CONFIG_MODE_Msk (0x7UL << INA231_CONFIG_MODE_Pos)

#define INA231_CONFIG_VSH_CONV_TIME_Pos (3)
#define INA231_CONFIG_VSH_CONV_TIME_Msk (0x7UL << INA231_CONFIG_VSH_CONV_TIME_Pos)

#define INA231_CONFIG_VBUS_CONV_TIME_Pos (6)
#define INA231_CONFIG_VBUS_CONV_TIME_Msk (0x7UL << INA231_CONFIG_VBUS_CONV_TIME_Pos)

#define INA231_CONFIG_AVG_Pos (9)
#define INA231_CONFIG_AVG_Msk (0x7UL << INA231_CONFIG_AVG_Pos)

#define INA231_CONFIG_MAGIC_ONE_Pos	(14)
#define INA231_CONFIG_MAGIC_ONE_Msk	(0x1UL << INA231_CONFIG_MAGIC_ONE_Pos)
#define INA231_CONFIG_MAGIC_ONE_Default (0x1UL)

#define INA231_CONFIG_RST_Pos (15)
#define INA231_CONFIG_RST_Msk (0x1UL << INA231_CONFIG_RST_Pos)

#define INA231_MASK_ENABLE_LEN_Pos    (0)
#define INA231_MASK_ENABLE_LEN_Msk    (0x1UL << INA231_MASK_ENABLE_LEN_Pos)
#define INA231_MASK_ENABLE_LEN_Enable (0x1UL)

#define INA231_MASK_ENABLE_APOL_Pos    (1)
#define INA231_MASK_ENABLE_APOL_Msk    (0x1UL << INA231_MASK_ENABLE_APOL_Pos)
#define INA231_MASK_ENABLE_APOL_Enable (0x1UL)

#define INA231_MASK_ENABLE_OVF_Pos    (2)
#define INA231_MASK_ENABLE_OVF_Msk    (0x1UL << INA231_MASK_ENABLE_OVF_Pos)
#define INA231_MASK_ENABLE_OVF_Enable (0x1UL)

#define INA231_MASK_ENABLE_CVRF_Pos    (3)
#define INA231_MASK_ENABLE_CVRF_Msk    (0x1UL << INA231_MASK_ENABLE_CVRF_Pos)
#define INA231_MASK_ENABLE_CVRF_Enable (0x1UL)

#define INA231_MASK_ENABLE_AFF_Pos    (4)
#define INA231_MASK_ENABLE_AFF_Msk    (0x1UL << INA231_MASK_ENABLE_AFF_Pos)
#define INA231_MASK_ENABLE_AFF_Enable (0x1UL)

#define INA231_MASK_ENABLE_CNVR_Pos    (10)
#define INA231_MASK_ENABLE_CNVR_Msk    (0x1UL << INA231_MASK_ENABLE_CNVR_Pos)
#define INA231_MASK_ENABLE_CNVR_Enable (0x1UL)

#define INA231_MASK_ENABLE_POL_Pos    (11)
#define INA231_MASK_ENABLE_POL_Msk    (0x1UL << INA231_MASK_ENABLE_POL_Pos)
#define INA231_MASK_ENABLE_POL_Enable (0x1UL)

#define INA231_MASK_ENABLE_BUL_Pos    (12)
#define INA231_MASK_ENABLE_BUL_Msk    (0x1UL << INA231_MASK_ENABLE_BUL_Pos)
#define INA231_MASK_ENABLE_BUL_Enable (0x1UL)

#define INA231_MASK_ENABLE_BOL_Pos    (13)
#define INA231_MASK_ENABLE_BOL_Msk    (0x1UL << INA231_MASK_ENABLE_BOL_Pos)
#define INA231_MASK_ENABLE_BOL_Enable (0x1UL)

#define INA231_MASK_ENABLE_SUL_Pos    (14)
#define INA231_MASK_ENABLE_SUL_Msk    (0x1UL << INA231_MASK_ENABLE_SUL_Pos)
#define INA231_MASK_ENABLE_SUL_Enable (0x1UL)

#define INA231_MASK_ENABLE_SOL_Pos    (15)
#define INA231_MASK_ENABLE_SOL_Msk    (0x1UL << INA231_MASK_ENABLE_SOL_Pos)
#define INA231_MASK_ENABLE_SOL_Enable (0x1UL)

/* Number of measurements INA231 will do an average based on */
typedef enum {
	INA231_CONFIG_AVG_1,
	INA231_CONFIG_AVG_4,
	INA231_CONFIG_AVG_16,
	INA231_CONFIG_AVG_64,
	INA231_CONFIG_AVG_128,
	INA231_CONFIG_AVG_256,
	INA231_CONFIG_AVG_512,
	INA231_CONFIG_AVG_1024,
	INA231_CONFIG_AVG_NO_OF_VALUES,
} ina231_config_avg_t;

/* Number of measurements INA231 will do an average based on */
static const int ina231_config_avg_to_num[] = { 1, 4, 16, 64, 128, 256, 512, 1024 };

/* VBUS conversion time */
typedef enum {
	INA231_CONFIG_VBUS_CONV_TIME_140_US,
	INA231_CONFIG_VBUS_CONV_TIME_204_US,
	INA231_CONFIG_VBUS_CONV_TIME_332_US,
	INA231_CONFIG_VBUS_CONV_TIME_588_US,
	INA231_CONFIG_VBUS_CONV_TIME_1100_US,
	INA231_CONFIG_VBUS_CONV_TIME_2116_US,
	INA231_CONFIG_VBUS_CONV_TIME_4156_US,
	INA231_CONFIG_VBUS_CONV_TIME_8244_US,
	INA231_CONFIG_VBUS_NO_OF_VALUES,
} ina231_config_vbus_conv_time_t;

/* VBUS conversion time us*/
static const int ina231_vbus_conv_time_to_us[] = { 140, 204, 332, 588, 1100, 2116, 4156, 8244 };

/* Volt over shunt resistor conversion time */
typedef enum {
	INA231_CONFIG_VSH_CONV_TIME_140_US,
	INA231_CONFIG_VSH_CONV_TIME_204_US,
	INA231_CONFIG_VSH_CONV_TIME_332_US,
	INA231_CONFIG_VSH_CONV_TIME_588_US,
	INA231_CONFIG_VSH_CONV_TIME_1100_US,
	INA231_CONFIG_VSH_CONV_TIME_2116_US,
	INA231_CONFIG_VSH_CONV_TIME_4156_US,
	INA231_CONFIG_VSH_CONV_TIME_8244_US,
	INA231_CONFIG_VSH_NO_OF_VALUES,
} ina231_config_vsh_conv_time_t;

/* Volt over shunt resistor conversion time */
static const int ina231_vsh_conv_time_to_us[] = { 140, 204, 332, 588, 1100, 2116, 4156, 8244 };

/* Measurement modes, one-shot or continuous */
typedef enum {
	INA231_CONFIG_MODE_POWER_DOWN,
	INA231_CONFIG_MODE_SHUNT_TRIG,
	INA231_CONFIG_MODE_BUS_TRIG,
	INA231_CONFIG_MODE_SHUNT_BUS_TRIG,
	INA231_CONFIG_MODE_POWER_DOWN_CONT,
	INA231_CONFIG_MODE_SHUNT_CONT,
	INA231_CONFIG_MODE_BUS_CONT,
	INA231_CONFIG_MODE_SHUNT_BUS_CONT,
} ina231_config_mode_t;

typedef struct {
	ina231_config_avg_t avg; /* Number of samples averaged */
	ina231_config_vbus_conv_time_t vbus_conv_time; /* Bus Voltage Conversion time */
	ina231_config_vsh_conv_time_t vsh_conv_time; /* Shunt Voltage Coversion Time */
	ina231_config_mode_t mode; /* Operating mode */
} ina231_config_reg_t;

/**@brief Open the INA231 driver.
 *
 * @return  0 if successful, ret if not
 */
int ina231_open(ina231_config_t const *p_cfg);

/**@brief Close the INA231 driver.
 *
 * @return  0 if successful, ret if not
 */
int ina231_close(ina231_config_t const *p_cfg);

/**@brief Reset the INA231 device.
 *
 * @return  0 if successful, ret if not
 */
int ina231_reset(void);

/**@brief Write configuration register
 *
 * @return  0 if successful, ret if not
 */
int ina231_config_set(ina231_config_reg_t *p_config);

/**@brief Read configuration register
 *
 * @return  0 if successful, ret if not
 */
int ina231_config_get(ina231_config_reg_t *p_config);

/**@brief Read shunt voltage register
 *
 * @return  0 if successful, ret if not
 */
int ina231_shunt_voltage_reg_get(uint16_t *p_voltage);

/**@brief Read bus voltage register
 *
 * @return  0 if successful, ret if not
 */
int ina231_bus_voltage_reg_get(uint16_t *p_voltage);

/**@brief Read power register
 *
 * @return  0 if successful, ret if not
 */
int ina231_power_reg_get(uint16_t *p_power);

/**@brief Read current register
 *
 * @return  0 if successful, ret if not
 */
int ina231_current_reg_get(uint16_t *p_current);

/**@brief Write calibration register
 *
 * @return  0 if successful, ret if not
 */
int ina231_calibration_set(uint16_t calib);

/**@brief Read calibration register
 *
 * @return  0 if successful, ret if not
 */
int ina231_calibration_get(uint16_t *p_calib);

/**@brief Write mask/enable register
 *
 * @return  0 if successful, ret if not
 */
int ina231_mask_enable_set(uint16_t mask_enable);

/**@brief Read mask/enable register
 *
 * @return  0 if successful, ret if not
 */
int ina231_mask_enable_get(uint16_t *p_mask_enable);

/**@brief Write alert limit register
 *
 * @return  0 if successful, ret if not
 */
int ina231_alert_limit_set(uint16_t alert_limit);

/**@brief Read alert limit register
 *
 * @return  0 if successful, ret if not
 */
int ina231_alert_limit_get(uint16_t *p_alert_limit);

/**@brief Convert enum value of conversion time to float
 *
 * @param  conv_time  Enum value to convert
 *
 * @return Conversion time in seconds
 */
float conv_time_enum_to_sec(ina231_config_vbus_conv_time_t conv_time);

/**@brief Convert enum value of numbers of average to uint16_t
 *
 * @param  average  Enum value to convert
 *
 * @return Number of readings to average over
 */
uint16_t average_enum_to_int(ina231_config_avg_t average);

#endif /* _INA231_H_ */
