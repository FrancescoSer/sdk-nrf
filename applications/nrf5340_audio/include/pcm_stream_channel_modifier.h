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

#ifndef _PCM_STREAM_CHANNEL_MODIFIER_H_
#define _PCM_STREAM_CHANNEL_MODIFIER_H_

#include <zephyr.h>

#include "sw_codec_select.h"

/**@brief  Adds a 0 after every sample from *input
 *	   and writes it to *output
 * @note: Use to create stereo stream from a mono source where one
 *	  channel is silent
 *
 * @param[in]	input:			Pointer to input buffer
 * @param[in]	input_size:		Number of bytes in input.
 * @param[in]	channel:		Channel to contain audio data
 * @param[in]	pcm_bit_depth		Bit depth of pcm samples (16, 24 or 32)
 * @param[out]	output:			Pointer to output buffer
 * @param[out]	output_size:		Number of bytes written to output.
 *
 * @return	0 if success
 */
int pscm_zero_pad(void const *const input, size_t input_size, audio_channel_t channel,
		  uint8_t pcm_bit_depth, void *output, size_t *output_size);

/**@brief  Adds a copy of every sample from *input
 *	   and writes it to both channels in *output
 * @note: Use to create stereo stream from a mono source where both
 *	  channels are identical
 *
 * @param[in]	input:			Pointer to input buffer
 * @param[in]	input_size:		Number of bytes in input.
 * @param[in]	pcm_bit_depth		Bit depth of pcm samples (16, 24 or 32)
 * @param[out]	output:			Pointer to output buffer
 * @param[out]	output_size:		Number of bytes written to output.
 *
 * @return	0 if success
 */
int pscm_copy_pad(void const *const input, size_t input_size, uint8_t pcm_bit_depth, void *output,
		  size_t *output_size);

/**@brief  Combines two mono streams into one stereo stream
 *
 * @param[in]	input_left:		Pointer to input buffer for left channel
 * @param[in]	input_right:		Pointer to input buffer for right channel
 * @param[in]	input_size:		Number of bytes in input. Same for both channels
 * @param[in]	pcm_bit_depth		Bit depth of pcm samples (16, 24 or 32)
 * @param[out]	output:			Pointer to output buffer
 * @param[out]	output_size:		Number of bytes written to output.
 *
 * @return	0 if success
 */
int pscm_combine(void const *const input_left, void const *const input_right, size_t input_size,
		 uint8_t pcm_bit_depth, void *output, size_t *output_size);

/**@brief  Removes every 2nd sample from *input
 *	   and writes it to *output
 * @note: Use to split stereo audio stream to single channel
 *
 * @param[in]	input:			Pointer to input buffer
 * @param[in]	input_size:		Number of bytes in input. Must be
 *					divisible by two
 * @param[in]	channel:		Channel to keep audio data from
 * @param[in]	pcm_bit_depth		Bit depth of pcm samples (16, 24 or 32)
 * @param[out]	output:			Pointer to output buffer
 * @param[out]	output_size:		Number of bytes written to output
 *
 * @return	0 if success.
 */
int pscm_one_channel_split(void const *const input, size_t input_size, audio_channel_t channel,
			   uint8_t pcm_bit_depth, void *output, size_t *output_size);

/**@brief  Splits a stereo stream to two separate mono streams
 * @note: Use to split stereo audio stream to two separate channels
 *
 * @param[in]	input:			Pointer to input buffer
 * @param[in]	input_size:		Number of bytes in input. Must be
 *					divisible by two
 * @param[in]	pcm_bit_depth		Bit depth of pcm samples (16, 24 or 32)
 * @param[out]	output_left:		Pointer to output buffer containing
 *					left channel
 * @param[out]	output_right:		Pointer to output buffer containing
 *					right channel
 * @param[out]	output_size:		Number of bytes written to output,
 *					same for both channels
 *
 * @return	0 if success.
 */
int pscm_two_channel_split(void const *const input, size_t input_size, uint8_t pcm_bit_depth,
			   void *output_left, void *output_right, size_t *output_size);

#endif /* _PCM_STREAM_CHANNEL_MODIFIER_H_ */
