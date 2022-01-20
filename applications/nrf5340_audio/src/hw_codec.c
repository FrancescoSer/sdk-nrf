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

#include "hw_codec.h"

#include <zephyr.h>
#include <stdlib.h>
#include <stdint.h>

#include "macros_common.h"
#include "cs47l63.h"
#include "cs47l63_spec.h"
#include "cs47l63_reg_conf.h"
#include "cs47l63_comm.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(HW_CODEC, CONFIG_LOG_HW_CODEC_LEVEL);

#define VOLUME_ADJUST_STEP_DB 3

static cs47l63_t cs47l63_driver;

/**@brief Write to multiple registers in CS47L63
 */
static int cs47l63_comm_reg_conf_write(const uint32_t config[][2], uint32_t num_of_regs)
{
	int ret;
	uint32_t reg;
	uint32_t value;

	for (int i = 0; i < num_of_regs; i++) {
		reg = config[i][0];
		value = config[i][1];

		if (reg == SPI_BUSY_WAIT) {
			LOG_DBG("Busy waiting instead of writing to CS47L63");
			/* Wait for us defined in value */
			k_busy_wait(value);
		} else {
			ret = cs47l63_write_reg(&cs47l63_driver, reg, value);
			RET_IF_ERR(ret);
		}
	}

	return 0;
}

int hw_codec_volume_set(uint16_t set_val)
{
	int ret;
	uint16_t volume_reg_val;

	volume_reg_val = set_val;
	if (volume_reg_val == 0) {
		LOG_WRN("Volume at MIN (-64dB)");
	} else if (volume_reg_val >= MAX_VOLUME_REG_VAL) {
		LOG_WRN("Volume at MAX (0dB)");
		volume_reg_val = MAX_VOLUME_REG_VAL;
	}

	ret = cs47l63_write_reg(&cs47l63_driver, CS47L63_OUT1L_VOLUME_1,
				volume_reg_val | CS47L63_OUT_VU);
	RET_IF_ERR(ret);
	return 0;
}

int hw_codec_volume_adjust(int8_t adjustment)
{
	int ret;
	static uint8_t prev_volume_reg_val = OUT_VOLUME_DEFAULT;

	if (adjustment == 0) {
		ret = cs47l63_write_reg(&cs47l63_driver, CS47L63_OUT1L_VOLUME_1,
					(prev_volume_reg_val | CS47L63_OUT_VU) &
						~CS47L63_OUT1L_MUTE);
		RET_IF_ERR(ret);

		return 0;
	}

	/* Get adjustment in dB, 1 bit is 0.5 dB,
	 * so multiply by 2 to get increments of 1 dB
	 */
	adjustment *= 2;

	uint16_t volume_reg_val;

	ret = cs47l63_read_reg(&cs47l63_driver, CS47L63_OUT1L_VOLUME_1,
			       (uint32_t *)&volume_reg_val);
	RET_IF_ERR(ret);

	volume_reg_val &= CS47L63_OUT1L_VOL_MASK;

	volume_reg_val += adjustment;
	if (volume_reg_val < 0) {
		LOG_WRN("Volume at MIN (-64dB)");
		volume_reg_val = 0;
	} else if (volume_reg_val > MAX_VOLUME_REG_VAL) {
		LOG_WRN("Volume at MAX (0dB)");
		volume_reg_val = MAX_VOLUME_REG_VAL;
	}

	ret = cs47l63_write_reg(&cs47l63_driver, CS47L63_OUT1L_VOLUME_1,
				(volume_reg_val | CS47L63_OUT_VU) & ~CS47L63_OUT1L_MUTE);
	RET_IF_ERR(ret);

	prev_volume_reg_val = volume_reg_val;
#if (CONFIG_LOG_HW_CODEC_LEVEL >= LOG_LEVEL_INF)
	int volume_in_db = (volume_reg_val / 2) - MAX_VOLUME_DB;

	/* This is rounded down to nearest integer */
	LOG_INF("Volume: %ddB", volume_in_db);
#endif /* (CONFIG_LOG_HW_CODEC_LEVEL >= LOG_LEVEL_INF) */

	return 0;
}

int hw_codec_volume_decrease(void)
{
	int ret;

	ret = hw_codec_volume_adjust(-VOLUME_ADJUST_STEP_DB);
	RET_IF_ERR(ret);

	return 0;
}

int hw_codec_volume_increase(void)
{
	int ret;

	ret = hw_codec_volume_adjust(VOLUME_ADJUST_STEP_DB);
	RET_IF_ERR(ret);

	return 0;
}

int hw_codec_volume_mute(void)
{
	int ret;
	uint16_t volume_reg_val;

	ret = cs47l63_read_reg(&cs47l63_driver, CS47L63_OUT1L_VOLUME_1,
			       (uint32_t *)&volume_reg_val);
	RET_IF_ERR(ret);

	BIT_SET(volume_reg_val, CS47L63_OUT1L_MUTE_MASK);

	ret = cs47l63_write_reg(&cs47l63_driver, CS47L63_OUT1L_VOLUME_1,
				volume_reg_val | CS47L63_OUT_VU);
	RET_IF_ERR(ret);

	return 0;
}

int hw_codec_volume_unmute(void)
{
	int ret;
	uint16_t volume_reg_val;

	ret = cs47l63_read_reg(&cs47l63_driver, CS47L63_OUT1L_VOLUME_1,
			       (uint32_t *)&volume_reg_val);
	RET_IF_ERR(ret);

	BIT_CLEAR(volume_reg_val, CS47L63_OUT1L_MUTE_MASK);

	ret = cs47l63_write_reg(&cs47l63_driver, CS47L63_OUT1L_VOLUME_1,
				volume_reg_val | CS47L63_OUT_VU);
	RET_IF_ERR(ret);

	return 0;
}

int hw_codec_default_conf_enable(void)
{
	int ret;

	ret = cs47l63_comm_reg_conf_write(clock_configuration, ARRAY_SIZE(clock_configuration));
	RET_IF_ERR(ret);

	ret = cs47l63_comm_reg_conf_write(GPIO_configuration, ARRAY_SIZE(GPIO_configuration));
	RET_IF_ERR(ret);

	ret = cs47l63_comm_reg_conf_write(asp1_enable, ARRAY_SIZE(asp1_enable));
	RET_IF_ERR(ret);

	ret = cs47l63_comm_reg_conf_write(output_enable, ARRAY_SIZE(output_enable));
	RET_IF_ERR(ret);

	ret = hw_codec_volume_adjust(0);
	RET_IF_ERR(ret);

#if ((NRF5340_AUDIO_DEV == NRF5340_AUDIO_DEV_GATEWAY) && (CONFIG_AUDIO_SOURCE_I2S))
	ret = cs47l63_comm_reg_conf_write(input_enable, ARRAY_SIZE(input_enable));
	RET_IF_ERR(ret);
#endif /* ((NRF5340_AUDIO_DEV == NRF5340_AUDIO_DEV_GATEWAY) && (CONFIG_AUDIO_SOURCE_I2S)) */

#if ((NRF5340_AUDIO_DEV == NRF5340_AUDIO_DEV_HEADSET) && CONFIG_STREAM_BIDIRECTIONAL)
	ret = cs47l63_comm_reg_conf_write(pdm_mic_enable_configure,
					  ARRAY_SIZE(pdm_mic_enable_configure));
	RET_IF_ERR(ret);
#endif /* ((NRF5340_AUDIO_DEV == NRF5340_AUDIO_DEV_HEADSET) && CONFIG_STREAM_BIDIRECTIONAL) */

	/* Toggle FLL to start up CS47L63 */
	ret = cs47l63_comm_reg_conf_write(FLL_toggle, ARRAY_SIZE(FLL_toggle));
	RET_IF_ERR(ret);

	return 0;
}

int hw_codec_soft_reset(void)
{
	int ret;

	ret = cs47l63_comm_reg_conf_write(output_disable, ARRAY_SIZE(output_disable));
	RET_IF_ERR(ret);

	ret = cs47l63_comm_reg_conf_write(soft_reset, ARRAY_SIZE(soft_reset));
	RET_IF_ERR(ret);

	return 0;
}

int hw_codec_init(void)
{
	int ret;

	ret = cs47l63_comm_init(&cs47l63_driver);
	ERR_CHK(ret);

	/* Run a soft reset on start to make sure all registers are default values */
	ret = cs47l63_comm_reg_conf_write(soft_reset, ARRAY_SIZE(soft_reset));
	RET_IF_ERR(ret);
	cs47l63_driver.state = CS47L63_STATE_STANDBY;

	return 0;
}
