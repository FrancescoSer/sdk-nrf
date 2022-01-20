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

#include "error_handler.h"

#include <zephyr.h>
#include <sys/reboot.h>
#include <fatal.h>
#include <logging/log_ctrl.h>

#include "board.h"
#include "led.h"

/* Print everything from the error handler */
#include <logging/log.h>
LOG_MODULE_REGISTER(error_handler, LOG_LEVEL_DBG);

static void led_error_indication(void)
{
#if defined(CONFIG_BOARD_NRF5340_AUDIO_DK_NRF5340_CPUAPP)
	(void)led_on(LED_APP_RGB, LED_COLOR_RED);
#elif defined(CONFIG_BOARD_NRF5340_AUDIO_DK_NRF5340_CPUNET)
	(void)led_on(LED_NET_RGB, LED_COLOR_RED);
#endif /* defined(CONFIG_BOARD_NRF5340_AUDIO_DK_NRF5340_CPUAPP) */
}

void error_handler(unsigned int reason, const z_arch_esf_t *esf)
{
#if (CONFIG_DEBUG)
	LOG_ERR("Caught system error -- reason %d. Entering infinite loop", reason);
	LOG_PANIC();
	led_error_indication();
	irq_lock();
	while (true) {
		__asm__ volatile("nop");
	}

#else
	LOG_ERR("Caught system error -- reason %d. Cold rebooting.", reason);
#if (CONFIG_LOG)
	LOG_PANIC();
#endif /* (CONFIG_LOG) */
	led_error_indication();
	sys_reboot(SYS_REBOOT_COLD);
#endif /* (CONFIG_DEBUG) */
	CODE_UNREACHABLE;
}

void bt_ctlr_assert_handle(char *c, int code)
{
	LOG_ERR("BT Controller assert: %s, code: 0x%x", c, code);
	error_handler(code, NULL);
}

void k_sys_fatal_error_handler(unsigned int reason, const z_arch_esf_t *esf)
{
	error_handler(reason, esf);
}

void assert_post_action(const char *file, unsigned int line)
{
	LOG_ERR("Assert post action: file: %s, line %d", file, line);
	error_handler(0, NULL);
}
