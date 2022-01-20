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

#include "fw_info_app.h"

#include <zephyr.h>
#include <stdio.h>
#include <logging/log_ctrl.h>
#include "channel_assignment.h"

#include "macros_common.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(fw_info);

static const char GIT_SHA1[]        = "8d8e6e21c-dirty";
static const char GIT_DATE[]        = "Tue Jan 18 09:35:05 2022";
static const char COMPILE_DATE[]    = "Thu Jan 20 14:55:39 2022";
static const char NRF5340_CORE[]    = "nRF5340 Audio nRF5340 Audio DK cpuapp";

#define VER_STRING_SIZE_MAX 12
static const uint8_t FW_VERSION_MAJOR = 0;
static const uint8_t FW_VERSION_MINOR = 4;
static const uint8_t FW_VERSION_PATCH = 0;
static char version_string[VER_STRING_SIZE_MAX];

int fw_info_app_print(void)
{
	int ret;

	ret = sprintf(version_string, "%d.%d.%d", FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_PATCH);
	if (ret < 0) {
		return ret;
	}

	LOG_INF(COLOR_GREEN"\r\n\t %s \
			    \r\n\t FW Version: %s \
			    \r\n\t Git info  : %s, %s\
			    \r\n\t Cmake run : %s"COLOR_RESET,
	NRF5340_CORE, version_string, GIT_DATE, GIT_SHA1, COMPILE_DATE);

#if (CONFIG_DEBUG)
	LOG_INF("------- DEBUG BUILD -------");

	#if (NRF5340_AUDIO_DEV==NRF5340_AUDIO_DEV_HEADSET)
		enum audio_channel channel;

		ret = channel_assignment_get(&channel);
		if (ret) {
			channel = AUDIO_CHANNEL_DEFAULT;
			static const char log_tag[] = "HL";
			ret = log_set_tag(log_tag);
			RET_IF_ERR(ret);
			LOG_INF(COLOR_CYAN"\r\n\t HEADSET <no ch selected> defaulting to " STRINGIFY(AUDIO_CHANNEL_DEFAULT)" "COLOR_RESET);
		}
		if (channel == AUDIO_CHANNEL_LEFT) {
			static const char log_tag[] = "HL";
			ret = log_set_tag(log_tag);
			RET_IF_ERR(ret);
			LOG_INF(COLOR_CYAN"\r\n\t HEADSET left device"COLOR_RESET);
		} else if (channel == AUDIO_CHANNEL_RIGHT) {
			static const char log_tag[] = "HR";
			ret = log_set_tag(log_tag);
			RET_IF_ERR(ret);
			LOG_INF(COLOR_CYAN"\r\n\t HEADSET right device"COLOR_RESET);
		} else {
			__ASSERT(false, "Unknown channel");
		}

	#elif NRF5340_AUDIO_DEV==NRF5340_AUDIO_DEV_GATEWAY
		static const char log_tag[] = "GW";
		ret = log_set_tag(log_tag);
		RET_IF_ERR(ret);
		LOG_INF(COLOR_CYAN"\r\n\t Compiled for GATEWAY device"COLOR_RESET);
	#endif /* (NRF5340_AUDIO_DEV==NRF5340_AUDIO_DEV_HEADSET) */
#endif /* (CONFIG_DEBUG) */

	return 0;
}
