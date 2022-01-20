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

#include "pcm_stream_channel_modifier.h"

#include <zephyr.h>
#include <errno.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(pscm);

/**
 * @brief      Determines whether the specified pcm bit depth is valid bit depth.
 *
 * @param[in]  pcm_bit_depth  The pcm bit depth
 *
 * @return     True if the specified pcm bit depth is valid bit depth, False otherwise.
 */
static bool is_valid_bit_depth(uint8_t pcm_bit_depth)
{
	if (pcm_bit_depth != 16 && pcm_bit_depth != 24 && pcm_bit_depth != 32) {
		LOG_ERR("Invalid bit depth: %d", pcm_bit_depth);
		return false;
	}

	return true;
}

/**
 * @brief      Determines if valid size.
 *
 * @param[in]  size              The size
 * @param[in]  bytes_per_sample  The bytes per sample
 * @param[in]  no_channels       No channels
 *
 * @return     True if valid size, False otherwise.
 */
static bool is_valid_size(size_t size, uint8_t bytes_per_sample, uint8_t no_channels)
{
	if (size % (bytes_per_sample * no_channels) != 0) {
		LOG_ERR("Size: %d is not dividable with number of bytes per sample x number of channels",
			size);
		return false;
	}

	return true;
}

int pscm_zero_pad(void const *const input, size_t input_size, audio_channel_t channel,
		  uint8_t pcm_bit_depth, void *output, size_t *output_size)
{
	uint8_t bytes_per_sample = pcm_bit_depth / 8;

	if (!is_valid_bit_depth(pcm_bit_depth) || !is_valid_size(input_size, bytes_per_sample, 1)) {
		return -EINVAL;
	}

	char *pointer_input = (char *)input;
	char *pointer_output = (char *)output;

	for (uint32_t i = 0; i < input_size / bytes_per_sample; i++) {
		if (channel == AUDIO_CH_L) {
			for (uint8_t j = 0; j < bytes_per_sample; j++) {
				*pointer_output++ = *pointer_input++;
			}

			for (uint8_t j = 0; j < bytes_per_sample; j++) {
				*pointer_output++ = 0;
			}
		} else if (channel == AUDIO_CH_R) {
			for (uint8_t j = 0; j < bytes_per_sample; j++) {
				*pointer_output++ = 0;
			}

			for (uint8_t j = 0; j < bytes_per_sample; j++) {
				*pointer_output++ = *pointer_input++;
			}
		} else {
			LOG_ERR("Invalid channel selection");
			return -EINVAL;
		}
	}

	*output_size = input_size * 2;
	return 0;
}

int pscm_copy_pad(void const *const input, size_t input_size, uint8_t pcm_bit_depth, void *output,
		  size_t *output_size)
{
	uint8_t bytes_per_sample = pcm_bit_depth / 8;

	if (!is_valid_bit_depth(pcm_bit_depth) || !is_valid_size(input_size, bytes_per_sample, 1)) {
		return -EINVAL;
	}

	char *pointer_input = (char *)input;
	char *pointer_output = (char *)output;

	for (uint32_t i = 0; i < input_size / bytes_per_sample; i++) {
		for (uint8_t j = 0; j < bytes_per_sample; j++) {
			*pointer_output++ = *pointer_input++;
		}
		/* Move back to start of sample to copy into next channel */
		pointer_input -= bytes_per_sample;

		for (uint8_t j = 0; j < bytes_per_sample; j++) {
			*pointer_output++ = *pointer_input++;
		}
	}

	*output_size = input_size * 2;
	return 0;
}

int pscm_combine(void const *const input_left, void const *const input_right, size_t input_size,
		 uint8_t pcm_bit_depth, void *output, size_t *output_size)
{
	uint8_t bytes_per_sample = pcm_bit_depth / 8;

	if (!is_valid_bit_depth(pcm_bit_depth) || !is_valid_size(input_size, bytes_per_sample, 1)) {
		return -EINVAL;
	}

	char *pointer_input_left = (char *)input_left;
	char *pointer_input_right = (char *)input_right;
	char *pointer_output = (char *)output;

	for (uint32_t i = 0; i < input_size / bytes_per_sample; i++) {
		for (uint8_t j = 0; j < bytes_per_sample; j++) {
			*pointer_output++ = *pointer_input_left++;
		}
		for (uint8_t j = 0; j < bytes_per_sample; j++) {
			*pointer_output++ = *pointer_input_right++;
		}
	}

	*output_size = input_size * 2;
	return 0;
}

int pscm_one_channel_split(void const *const input, size_t input_size, audio_channel_t channel,
			   uint8_t pcm_bit_depth, void *output, size_t *output_size)
{
	uint8_t bytes_per_sample = pcm_bit_depth / 8;

	if (!is_valid_bit_depth(pcm_bit_depth) || !is_valid_size(input_size, bytes_per_sample, 2)) {
		return -EINVAL;
	}

	char *pointer_input = (char *)input;
	char *pointer_output = (char *)output;

	for (uint32_t i = 0; i < input_size / bytes_per_sample; i += 2) {
		if (channel == AUDIO_CH_L) {
			for (uint8_t j = 0; j < bytes_per_sample; j++) {
				*pointer_output++ = *pointer_input++;
			}
			pointer_input += bytes_per_sample;

		} else if (channel == AUDIO_CH_R) {
			pointer_input += bytes_per_sample;

			for (uint8_t j = 0; j < bytes_per_sample; j++) {
				*pointer_output++ = *pointer_input++;
			}
		} else {
			LOG_ERR("Invalid channel selection");
			return -EINVAL;
		}
	}

	*output_size = input_size / 2;
	return 0;
}

int pscm_two_channel_split(void const *const input, size_t input_size, uint8_t pcm_bit_depth,
			   void *output_left, void *output_right, size_t *output_size)
{
	uint8_t bytes_per_sample = pcm_bit_depth / 8;

	if (!is_valid_bit_depth(pcm_bit_depth) || !is_valid_size(input_size, bytes_per_sample, 2)) {
		return -EINVAL;
	}

	char *pointer_input = (char *)input;
	char *pointer_output_left = (char *)output_left;
	char *pointer_output_right = (char *)output_right;

	for (uint32_t i = 0; i < input_size / bytes_per_sample; i += 2) {
		for (uint8_t j = 0; j < bytes_per_sample; j++) {
			*pointer_output_left++ = *pointer_input++;
		}
		for (uint8_t j = 0; j < bytes_per_sample; j++) {
			*pointer_output_right++ = *pointer_input++;
		}
	}

	*output_size = input_size / 2;
	return 0;
}
