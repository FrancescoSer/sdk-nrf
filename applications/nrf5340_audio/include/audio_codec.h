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

#ifndef _AUDIO_CODEC_H_
#define _AUDIO_CODEC_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Toggle a test tone on and off
 *
 * @param[in] freq Desired frequency of tone, 0 = off
 *
 * @note A stream must already be running to use this feature
 *
 * @return 0 on success, and -ENOMEM if the frequency is too low (buffer overflow)
 */
int audio_encode_test_tone_set(uint32_t freq);

/**
 * @brief Decode data and then add it to TX FIFO buffer
 *
 * @param[in]	encoded_data		Pointer to encoded data
 * @param[in]	encoded_data_size	Size of encoded data
 * @param[in]	bad_frame		Indication on missed or incomplete frame
 *
 * @return 0 on success, error otherwise
 */
int audio_decode(void const *const encoded_data, size_t encoded_data_size, bool bad_frame);

/**
 * @brief Initialize and start audio on gateway
 */
void audio_gateway_start(void);

/**
 * @brief Initialize and start audio on headset
 */
void audio_headset_start(void);

/**
 * @brief Stop all activities related to audio
 */
void audio_stop(void);

#endif /* _AUDIO_CODEC_H_ */
