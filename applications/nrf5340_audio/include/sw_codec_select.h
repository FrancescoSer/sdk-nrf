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

#ifndef _SW_CODEC_SELECT_H_
#define _SW_CODEC_SELECT_H_

#include <zephyr.h>

#if (CONFIG_SW_CODEC_SBC)
#include "sbc_encoder.h"
#define SBC_ENC_MONO_FRAME_SIZE (CONFIG_SW_CODEC_SBC_MONO_BITRATE * SBC_FRAME_SIZE_MS / (8 * 1000))
#define PCM_NUM_BYTES_SBC_FRAME_MONO \
	(CONFIG_AUDIO_SAMPLE_RATE_HZ * (CONFIG_AUDIO_CONTAINER_BITS / 8) * SBC_FRAME_SIZE_MS / 1000)

#define SBC_PCM_NUM_BYTES_MONO \
	(PCM_NUM_BYTES_SBC_FRAME_MONO * CONFIG_SW_CODEC_SBC_NUM_FRAMES_PER_BLE_PACKET)

#define SBC_ENC_MAX_FRAME_SIZE \
	(SBC_ENC_MONO_FRAME_SIZE * CONFIG_SW_CODEC_SBC_NUM_FRAMES_PER_BLE_PACKET)

#else
#define SBC_ENC_MAX_FRAME_SIZE 0
#define SBC_PCM_NUM_BYTES_MONO 0
#endif /* CONFIG_SW_CODEC_SBC */

#if (CONFIG_SW_CODEC_LC3)
#define LC3_MAX_FRAME_SIZE_MS 10
#define LC3_ENC_MONO_FRAME_SIZE \
	(CONFIG_SW_CODEC_LC3_MONO_BITRATE * LC3_MAX_FRAME_SIZE_MS / (8 * 1000))

#define LC3_PCM_NUM_BYTES_MONO \
	(CONFIG_AUDIO_SAMPLE_RATE_HZ * (CONFIG_AUDIO_CONTAINER_BITS / 8) * LC3_MAX_FRAME_SIZE_MS / \
	 1000)
#else
#define LC3_ENC_MONO_FRAME_SIZE 0
#define LC3_PCM_NUM_BYTES_MONO	0
#endif /* CONFIG_SW_CODEC_LC3 */

#define ENC_MAX_FRAME_SIZE   MAX(LC3_ENC_MONO_FRAME_SIZE, SBC_ENC_MAX_FRAME_SIZE)
#define PCM_NUM_BYTES_MONO   MAX(LC3_PCM_NUM_BYTES_MONO, SBC_PCM_NUM_BYTES_MONO)
#define PCM_NUM_BYTES_STEREO (PCM_NUM_BYTES_MONO * 2)

typedef enum {
	SW_CODEC_NONE,
	SW_CODEC_LC3, /**< Low Complexity Communication Codec */
	SW_CODEC_SBC, /**< Subband codec */
} sw_codec_select_t;

typedef enum {
	SW_CODEC_ZERO_CHANNELS,
	SW_CODEC_MONO, /**< Only use one channel */
	SW_CODEC_STEREO, /**< Use both channels */
} sw_codec_select_ch_t;

typedef enum {
	AUDIO_CH_L,
	AUDIO_CH_R,
	AUDIO_CH_NUM,
} audio_channel_t;

typedef struct {
	bool enabled;
	int bitrate;
	sw_codec_select_ch_t channel_mode;
	audio_channel_t audio_ch; /**< Only used if channel mode is mono */
} sw_codec_encoder_t;

typedef struct {
	bool enabled;
	sw_codec_select_ch_t channel_mode;
	audio_channel_t audio_ch; /**< Only used if channel mode is mono */
} sw_codec_decoder_t;

/** @brief  Sw_codec configuration structure
 */
typedef struct {
	sw_codec_select_t sw_codec; /**< sw_codec to be used, e.g. LC3, SBC etc */
	sw_codec_decoder_t decoder; /**< Struct containing settings for decoder */
	sw_codec_encoder_t encoder; /**< Struct containing settings for encoder */
	bool initialized; /**< Status of codec */
} sw_codec_config_t;

/**@brief	Encode PCM data and output encoded data
 *
 * @note	Takes in stereo PCM stream, will encode either one or two
 *		channels, based on channel_mode set during init
 *
 * @param[in]	pcm_data	Pointer to PCM data
 * @param[in]	pcm_size	Size of PCM data
 * @param[out]	encoded_data	Pointer to buffer to store encoded data
 * @param[out]	encoded_size	Size of encoded data
 *
 * @return	0 if success, error codes depends on sw_codec selected
 */
int sw_codec_encode(void *pcm_data, size_t pcm_size, uint8_t **encoded_data, size_t *encoded_size);

/**@brief	Decode encoded data and output PCM data
 *
 * @param[in]	encoded_data	Pointer to encoded data
 * @param[in]	encoded_size	Size of encoded data
 * @param[in]	bad_frame	Flag to indicate a missing/bad frame (only LC3)
 * @param[out]	pcm_data	Pointer to buffer to store decoded PCM data
 * @param[out]	pcm_size	Size of decoded data
 *
 * @return	0 if success, error codes depends on sw_codec selected
 */
int sw_codec_decode(uint8_t const *const encoded_data, size_t encoded_size, bool bad_frame,
		    void **pcm_data, size_t *pcm_size);

/**@brief	Uninitialize sw_codec and free allocated space
 *
 * @note	Must be called before calling init for another sw_codec
 *
 * @param[in]	sw_codec_config	Struct to tear down sw_codec
 *
 * @return	0 if success, error codes depends on sw_codec selected
 */
int sw_codec_uninit(sw_codec_config_t sw_codec_config);

/**@brief	Initialize sw_codec and statically or dynamically
 *		allocate memory to be used, depending on selected codec
 *		and its configuration.
 *
 * @param[in]	sw_codec_config	Struct to set up sw_codec
 *
 * @return	0 if success, error codes depends on sw_codec selected
 */
int sw_codec_init(sw_codec_config_t sw_codec_config);

#endif /* _SW_CODEC_SELECT_H_ */
