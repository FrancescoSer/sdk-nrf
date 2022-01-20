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

#include "pmic.h"

#include <zephyr.h>
#include <stdio.h>
#include <device.h>
#include <logging/log_ctrl.h>

#include "max14690.h"
#include "macros_common.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(pmic, CONFIG_LOG_PMIC_LEVEL);

int pmic_defaults_set(void)
{
	int ret;

	ret = max14690_boot_cfg_stay_on_cfg(PMIC_PWR_CFG_STAY_ON_ENABLED);
	RET_IF_ERR(ret);
	ret = max14690_mtn_chg_tmr_cfg(PMIC_MTN_CHG_TMR_60_MIN);
	RET_IF_ERR(ret);
	ret = max14690_pmic_thrm_cfg(PMIC_THRM_CFG_THERM_ENABLED_JEITA_ENABLED);
	RET_IF_ERR(ret);

	return 0;
}

int pmic_pwr_off(void)
{
	LOG_WRN("PMIC powering off");
	log_panic();
	return max14690_pwr_off();
}

int pmic_init(void)
{
	int ret;

	const struct device *i2c_dev = device_get_binding("I2C_1");

	ret = max14690_init(i2c_dev);
	RET_IF_ERR(ret);

	return 0;
}
