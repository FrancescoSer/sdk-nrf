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

#include "contin_array.h"

#include <zephyr.h>
#include <stdio.h>
#include <string.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(contin_array, LOG_LEVEL_WRN);

int contin_array_create(void *const pcm_cont, uint32_t pcm_cont_size, void const *const pcm_finite,
			uint32_t pcm_finite_size, uint32_t *const finite_pos)
{
	LOG_DBG("pcm_cont_size: %d pcm_finite_size %d", pcm_cont_size, pcm_finite_size);

	if (pcm_cont == NULL || pcm_finite == NULL) {
		return -ENXIO;
	}

	if (!pcm_cont_size || !pcm_finite_size) {
		LOG_ERR("size cannot be zero");
		return -EPERM;
	}

	for (uint32_t i = 0; i < pcm_cont_size; i++) {
		if (*finite_pos > (pcm_finite_size - 1)) {
			*finite_pos = 0;
		}
		((char *)pcm_cont)[i] = ((char *)pcm_finite)[*finite_pos];
		(*finite_pos)++;
	}

	return 0;
}
