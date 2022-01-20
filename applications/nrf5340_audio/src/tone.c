/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
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

#include "tone.h"

#include <zephyr.h>
#include <math.h>
#include <stdio.h>
#include <arm_math.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(tone, CONFIG_LOG_AUDIO_CODEC_LEVEL);

#define FREQ_LIMIT_LOW	100
#define FREQ_LIMIT_HIGH 10000

int tone_gen(int16_t *tone, size_t *tone_size, uint16_t tone_freq_hz, uint32_t smpl_freq_hz,
	     float amplitude)
{
	if (tone == NULL) {
		return -ENXIO;
	}

	if (!smpl_freq_hz || tone_freq_hz < FREQ_LIMIT_LOW || tone_freq_hz > FREQ_LIMIT_HIGH) {
		return -EINVAL;
	}

	if (amplitude > 1 || amplitude <= 0) {
		return -EPERM;
	}

	uint32_t samples_for_one_period = smpl_freq_hz / tone_freq_hz;

	for (uint32_t i = 0; i < samples_for_one_period; i++) {
		float curr_val = i * 2 * PI / samples_for_one_period;
		float32_t res = arm_sin_f32(curr_val);
		/* Generate one sine wave */
		tone[i] = amplitude * res * INT16_MAX;
	}

	/* Configured for bit depth 16 */
	*tone_size = (size_t)samples_for_one_period * 2;

	return 0;
}
