/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
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

#include "channel_assignment.h"

#include <errno.h>

#include "uicr.h"

int channel_assignment_get(enum audio_channel *channel)
{
#if CONFIG_AUDIO_HEADSET_CHANNEL_RUNTIME
	uint8_t channel_value;

	channel_value = uicr_channel_get();

	if (channel_value >= AUDIO_CHANNEL_COUNT) {
		return -EIO;
	}

	*channel = (enum audio_channel)channel_value;
#else
	*channel = (enum audio_channel)CONFIG_AUDIO_HEADSET_CHANNEL;
#endif /* CONFIG_AUDIO_HEADSET_CHANNEL_RUNTIME */

	return 0;
}

#if CONFIG_AUDIO_HEADSET_CHANNEL_RUNTIME
/**
 * @brief Assign audio channel.
 *
 * @param[out] channel Channel value
 *
 * @return 0 if successful
 * @return -EROFS if different channel is already written
 * @return -EIO if channel is not assigned.
 */
int channel_assignment_set(enum audio_channel channel)
{
	return uicr_channel_set(channel);
}

#endif /* CONFIG_AUDIO_HEADSET_CHANNEL_RUNTIME */
