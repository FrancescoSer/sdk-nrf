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

#ifndef _CS47L63_REG_CONF_H_
#define _CS47L63_REG_CONF_H_

#include "cs47l63_spec.h"

/* Magic value to signal a sleep instead of register address.
 * This can be used e.g. after resets where time is needed before
 * a device is ready.
 * Note that this is a busy wait, and should only be used sparingly where fast
 * execution is not critical.
 *
 * 0001 is used as the reg addr. In case of a fault, this reg is read only.
 */
#define SPI_BUSY_WAIT	      0x0001
#define SPI_BUSY_WAIT_US_1000 1000
#define SPI_BUSY_WAIT_US_3000 3000

#define MAX_VOLUME_REG_VAL 0x80
#define MAX_VOLUME_DB	   64
#define OUT_VOLUME_DEFAULT 0x62
#define VOLUME_UPDATE_BIT  (1 << 9)

#define CS47L63_SOFT_RESET_VAL 0x5A000000

/* clang-format off */
/* Set up clocks */
const uint32_t clock_configuration[][2] = {
	{ CS47L63_SYSTEM_CLOCK1, 0x034C },
	{ CS47L63_ASYNC_CLOCK1, 0x034C },
	{ CS47L63_FLL1_CONTROL2, 0x88608020 },
	{ CS47L63_FLL1_CONTROL3, 0x10000 },
	{ CS47L63_FLL1_GPIO_CLOCK, 0x0005 },
	{ CS47L63_FLL1_CONTROL1, 0x0001 },
};
/* clang-format on */

/* Set up GPIOs */
const uint32_t GPIO_configuration[][2] = {
	{ CS47L63_GPIO6_CTRL1, 0x61000001 },
	{ CS47L63_GPIO7_CTRL1, 0x61000001 },
	{ CS47L63_GPIO8_CTRL1, 0x61000001 },

	/* Enable CODEC LED */
	{ CS47L63_GPIO10_CTRL1, 0x41008001 },
};

const uint32_t pdm_mic_enable_configure[][2] = {
	/* Set MICBIASes */
	{ CS47L63_LDO2_CTRL1, 0x0005 },
	{ CS47L63_MICBIAS_CTRL1, 0x00EC },
	{ CS47L63_MICBIAS_CTRL5, 0x0272 },

	/* Enable IN1L */
	{ CS47L63_INPUT_CONTROL, 0x000E },

	/* Enable PDM mic as digital input */
	{ CS47L63_INPUT1_CONTROL1, 0x50021 },

	/* Un-mute and set gain to 0dB */
	{ CS47L63_IN1L_CONTROL2, 0x800080 },
	{ CS47L63_IN1R_CONTROL2, 0x800080 },

	/* Volume Update */
	{ CS47L63_INPUT_CONTROL3, 0x20000000 },

	/* Send PDM MIC to I2S Tx */
	{ CS47L63_ASP1TX1_INPUT1, 0x800010 },
	{ CS47L63_ASP1TX2_INPUT1, 0x800011 },
};

/* Set up input */
const uint32_t input_enable[][2] = {
	/* Select LINE-IN as analog input */
	{ CS47L63_INPUT2_CONTROL1, 0x50020 },

	/* Set IN2L and IN2R to single-ended */
	{ CS47L63_IN2L_CONTROL1, 0x10000000 },
	{ CS47L63_IN2R_CONTROL1, 0x10000000 },

	/* Un-mute and set gain to 0dB */
	{ CS47L63_IN2L_CONTROL2, 0x800080 },
	{ CS47L63_IN2R_CONTROL2, 0x800080 },

	/* Enable IN2L and IN2R */
	{ CS47L63_INPUT_CONTROL, 0x000E },

	/* Volume Update */
	{ CS47L63_INPUT_CONTROL3, 0x20000000 },

	/* Route IN2L and IN2R to I2S */
	{ CS47L63_ASP1TX1_INPUT1, 0x800012 },
	{ CS47L63_ASP1TX2_INPUT1, 0x800013 },
};

/* Set up output */
const uint32_t output_enable[][2] = {
	{ CS47L63_OUTPUT_ENABLE_1, 0x0002 },

	{ CS47L63_OUT1L_INPUT1, 0x800020 },
	{ CS47L63_OUT1L_INPUT2, 0x800021 },
};

const uint32_t output_disable[][2] = {
	{ CS47L63_OUTPUT_ENABLE_1, 0x00 },
};

/* Set up ASP1 (I2S) */
const uint32_t asp1_enable[][2] = {
	/* Enable ASP1 GPIOs */
	{ CS47L63_GPIO1_CTRL1, 0x61000000 },
	{ CS47L63_GPIO2_CTRL1, 0xE1000000 },
	{ CS47L63_GPIO3_CTRL1, 0xE1000000 },
	{ CS47L63_GPIO4_CTRL1, 0xE1000000 },
	{ CS47L63_GPIO5_CTRL1, 0x61000001 },

	/* Set ASP1 in slave mode and 16 bit per channel */
	{ CS47L63_ASP1_CONTROL1, 0x001B },
	{ CS47L63_ASP1_CONTROL2, 0x10100200 },
	{ CS47L63_ASP1_CONTROL3, 0x0000 },
	{ CS47L63_ASP1_DATA_CONTROL1, 0x0020 },
	{ CS47L63_ASP1_DATA_CONTROL5, 0x0020 },
	{ CS47L63_ASP1_ENABLES1, 0x30003 },
};

const uint32_t FLL_toggle[][2] = {
	{ CS47L63_FLL1_CONTROL1, 0x0000 },
	{ SPI_BUSY_WAIT, SPI_BUSY_WAIT_US_1000 },
	{ CS47L63_FLL1_CONTROL1, 0x0001 },
};

const uint32_t soft_reset[][2] = {
	{ CS47L63_SFT_RESET, CS47L63_SOFT_RESET_VAL },
	{ SPI_BUSY_WAIT, SPI_BUSY_WAIT_US_3000 },
};

#endif /* _CS47L63_REG_CONF_H_ */
