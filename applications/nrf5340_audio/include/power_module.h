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

#ifndef _POWER_MODULE_H_
#define _POWER_MODULE_H_

#include <zephyr.h>
#include <stdlib.h>

#include "ina231.h"

#define RAIL_NAME_MAX_SIZE 20

typedef void (*nrf_power_module_handler_t)(uint8_t rail);

/*
 * Different rails to be measured by the INA231
 * VBAT		= Battery
 * VDD1_CODEC	= 1.2V rail for CS47L63
 * VDD2_CODEC	= 1.8V rail for CS47L63
 * VDD2_NRF	= NRF5340
 */
typedef enum {
	VBAT,
	VDD1_CODEC,
	VDD2_CODEC,
	VDD2_NRF,
} ina_name_t;

typedef struct {
	float current;
	float power;
	float bus_voltage;
	float shunt_voltage;
} power_module_data_t;

typedef struct {
	uint8_t address;
	ina231_config_reg_t config;
	power_module_data_t meas_data;
	nrf_power_module_handler_t callback;
	float power_lsb;
	float current_lsb;
	char name[RAIL_NAME_MAX_SIZE];
} ina231_t;

/**@brief   Read the latest measurements from a given INA231
 *
 * @param   name    Name of the INA231 to be read from
 * @param   data    Container for the read data
 */
void power_module_data_get(ina_name_t name, power_module_data_t *data);

/**@brief   Write configuration to INA231 to start measurements
 *
 * @param   name    Name of the INA231 to start
 *
 * @param   data_handler    Callback to use when data is ready, if set to NULL
 *			    the default callback for printing measurements will
 *			    be used
 *
 * @return  0 if successful
 */
int power_module_measurement_start(ina_name_t name, nrf_power_module_handler_t data_handler);

/**@brief   Stop continuous measurements from given INA231
 *
 * @param   name    Name of the INA231 to be stopped
 *
 * @return  0 if successful
 */
int power_module_measurement_stop(ina_name_t name);

/**@brief   Getter for average value of the INAs
 *
 * @return  value of average setting
 */
uint8_t power_module_avg_get(void);

/**@brief   Setter for average value of the INAs
 *
 * @param   avg     value of average setting
 *
 * @note A given INA must be restarted for the new value to take effect.
 *
 * @return  0 if successful
 */
int power_module_avg_set(uint8_t avg);

/**@brief   Getter for conversion time value of the INAs
 *
 * @return  value of conversion time setting
 */
uint8_t power_module_conv_time_get(void);

/**@brief   Setter for conversion time value of the INAs
 *
 * @param   conv_time     value of conversion time setting
 *
 * @note A given INA must be restarted for the new value to take effect.
 *
 * @return  0 if successful
 */
int power_module_conv_time_set(uint8_t conv_time);

/**@brief   Initialize power module
 *
 * @return  0 if successful
 */
int power_module_init(void);

#endif /* _POWER_MODULE_H_ */
