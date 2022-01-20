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

#ifndef _HW_CODEC_H_
#define _HW_CODEC_H_

#include <stdint.h>

/**
 * @brief  Set volume on HW_CODEC
 *
 * @details Also unmute the volume on HW_CODEC
 *
 * @param  set_val  Set the volume to a specific value.
 *                  This range of the value is between 0 to 128.
 *
 * @return 0 if successful, error otherwise
 */
int hw_codec_volume_set(uint16_t set_val);

/**
 * @brief  Adjust volume on HW_CODEC
 *
 * @details Also unmute the volume on HW_CODEC
 *
 * @param  adjustment  The adjustment in dB, can be negative or positive.
 *			If the value 0 is used, the previous known value will be
 *			written, default value will be used if no previous value
 *			exists
 *
 * @return 0 if successful, error otherwise
 */
int hw_codec_volume_adjust(int8_t adjustment);

/**
 * @brief Decrease output volume on HW_CODEC by 3 dB
 *
 * @details Also unmute the volume on HW_CODEC
 *
 * @return 0 if successful, error otherwise
 */
int hw_codec_volume_decrease(void);

/**
 * @brief Increase output volume on HW_CODEC by 3 dB
 *
 * @details Also unmute the volume on HW_CODEC
 *
 * @return 0 if successful, error otherwise
 */
int hw_codec_volume_increase(void);

/**
 * @brief  Mute volume on HW_CODEC
 *
 * @return 0 if successful, error otherwise
 */
int hw_codec_volume_mute(void);

/**
 * @brief  Unmute volume on HW_CODEC
 *
 * @return 0 if successful, error otherwise
 */
int hw_codec_volume_unmute(void);

/**
 * @brief Enable relevant settings in HW_CODEC to
 *        send and receive PCM data over I2S
 *
 * @note  FLL1 must be toggled after I2S has started to enable HW_CODEC
 *
 * @return 0 if successful, error otherwise
 */
int hw_codec_default_conf_enable(void);

/**
 * @brief Reset HW_CODEC
 *
 * @note  This will first disable output, then do a soft reset
 *
 * @return 0 if successful, error otherwise
 */
int hw_codec_soft_reset(void);

/**
 * @brief Initialize HW_CODEC
 *
 * @return 0 if successful, error otherwise
 */
int hw_codec_init(void);

#endif /* _HW_CODEC_H_ */
