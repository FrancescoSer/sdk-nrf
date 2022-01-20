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

#include "ctrl_events.h"

#include <zephyr.h>
#include <kernel.h>
#include <errno.h>

#include "macros_common.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(ctrl_events, CONFIG_LOG_CTRL_EVENTS_LEVEL);

#define CTRL_EVENTS_MSGQ_MAX_ELEMENTS	 5
#define CTRL_EVENTS_MSGQ_ALIGNMENT_WORDS 4

K_MSGQ_DEFINE(msg_queue, sizeof(struct event_t), CTRL_EVENTS_MSGQ_MAX_ELEMENTS,
	      CTRL_EVENTS_MSGQ_ALIGNMENT_WORDS);

bool ctrl_events_queue_empty(void)
{
	uint32_t num_free;

	num_free = k_msgq_num_free_get(&msg_queue);

	return (num_free == CTRL_EVENTS_MSGQ_MAX_ELEMENTS ? true : false);
}

int ctrl_events_put(struct event_t *event)
{
	if (event == NULL) {
		LOG_ERR("Event is NULL. Event source: %d", event->event_source);
		return -EFAULT;
	}

	return k_msgq_put(&msg_queue, (void *)event, K_NO_WAIT);
}

int ctrl_events_get(struct event_t *my_event, k_timeout_t timeout)
{
	return k_msgq_get(&msg_queue, (void *)my_event, timeout);
}
