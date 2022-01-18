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

#ifndef __BOARD_H__
#define __BOARD_H__

#include <zephyr.h>

/* Voltage divider PCA10121 board versions.
 * The defines give what value the ADC will read back.
 * This is determined by the on-board voltage divider.
 */

typedef struct {
	char name[10];
	uint32_t mask;
	uint32_t adc_reg_val;
} board_version_t;

#define BOARD_PCA10121_0_0_0_MSK (BIT(0))
#define BOARD_PCA10121_0_6_0_MSK (BIT(1))
#define BOARD_PCA10121_0_7_0_MSK (BIT(2))
#define BOARD_PCA10121_0_7_1_MSK (BIT(3))
#define BOARD_PCA10121_0_8_0_MSK (BIT(4))
#define BOARD_PCA10121_0_8_1_MSK (BIT(5))
#define BOARD_PCA10121_0_8_2_MSK (BIT(6))
#define BOARD_PCA10121_0_9_0_MSK (BIT(7))
#define BOARD_PCA10121_0_10_0_MSK (BIT(8))

static const board_version_t BOARD_VERSION_ARR[] = {
	{ "0.0.0", BOARD_PCA10121_0_0_0_MSK, INT_MIN },
	{ "0.6.0", BOARD_PCA10121_0_6_0_MSK, 61 },
	{ "0.7.0", BOARD_PCA10121_0_7_0_MSK, 102 },
	{ "0.7.1", BOARD_PCA10121_0_7_1_MSK, 303 },
	{ "0.8.0", BOARD_PCA10121_0_8_0_MSK, 534 },
	{ "0.8.1", BOARD_PCA10121_0_8_1_MSK, 780 },
	{ "0.8.2", BOARD_PCA10121_0_8_2_MSK, 1018 },
	{ "0.9.0", BOARD_PCA10121_0_9_0_MSK, 1260 },
	/* Lower value used on 0.10.0 due to high ohm divider */
	{ "0.10.0", BOARD_PCA10121_0_10_0_MSK, 1480 },
};

#define BOARD_VERSION_VALID_MSK                                                                    \
	(BOARD_PCA10121_0_7_0_MSK | BOARD_PCA10121_0_7_1_MSK | BOARD_PCA10121_0_8_0_MSK |          \
	 BOARD_PCA10121_0_8_1_MSK | BOARD_PCA10121_0_8_2_MSK | BOARD_PCA10121_0_9_0_MSK |          \
	 BOARD_PCA10121_0_10_0_MSK)

#define BOARD_VERSION_VALID_MSK_SD_CARD                                                            \
	(BOARD_PCA10121_0_7_0_MSK | BOARD_PCA10121_0_8_0_MSK | BOARD_PCA10121_0_8_1_MSK |          \
	 BOARD_PCA10121_0_8_2_MSK | BOARD_PCA10121_0_9_0_MSK | BOARD_PCA10121_0_10_0_MSK)

#define BOARD_REVISION_VALID_MSK_MAX14690_PMIC                                                     \
	(BOARD_PCA10121_0_6_0_MSK | BOARD_PCA10121_0_7_0_MSK | BOARD_PCA10121_0_7_1_MSK)

#define LED_APP_RGB 0
#define LED_NET_RGB 1
#define LED_APP_1_BLUE 2
#define LED_APP_2_GREEN 3
#define LED_APP_3_GREEN 4

#if !defined(NRF5340_AUDIO_DEV)
#error NRF5340_AUDIO_DEV has not been defined
#endif

#if !defined(NRF5340_AUDIO_DEV_HEADSET)
#error NRF5340_AUDIO_DEV_HEADSET has not been defined
#endif

#if !defined(NRF5340_AUDIO_DEV_GATEWAY)
#error NRF5340_AUDIO_DEV_GATEWAY has not been defined
#endif

#if ((NRF5340_AUDIO_DEV != NRF5340_AUDIO_DEV_HEADSET) &&                                           \
     (NRF5340_AUDIO_DEV != NRF5340_AUDIO_DEV_GATEWAY))
#error NRF5340_AUDIO_DEV must be set to either NRF5340_AUDIO_DEV_HEADSET or \
       NRF5340_AUDIO_DEV_GATEWAY
#endif

#endif
