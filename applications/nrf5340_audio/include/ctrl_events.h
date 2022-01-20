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

/** \addtogroup streamctrl Stream control - control transfer and processing of the audio streams
 *  @{
 */

/** @file ctrl_events.h
 *  @brief Control events - events used for transfer and processing of the audio streams
 *
 */

#ifndef _CTRL_EVENTS_H_
#define _CTRL_EVENTS_H_

#include <zephyr.h>

#include "ble_trans.h"
#include "button_handler.h"

/** @brief Event sources
 *
 * These are the sources that can post activity events to the queue.
 */
typedef enum {
	EVT_SRC_BUTTON,
	EVT_SRC_PEER,
} event_source_t;

/** @brief Events for activity from event sources
 *
 * This type wraps activity/events from the various event
 * sources, with an identification of the source, for
 * posting to the event queue.
 */
struct event_t {
	event_source_t event_source;

	union {
		button_evt_t button_activity;
		ble_evt_type_t link_activity;
	};
};

/**@brief  Check if event queue is empty
 *
 * @retval True if queue is empty, false if not
 */
bool ctrl_events_queue_empty(void);

/**@brief  Put event in k_msgq
 *
 * @param  event   Pointer to event
 *
 * @retval 0 Event sent.
 * @retval -EFAULT Try to send event with address NULL
 * @retval -ENOMSG Returned without waiting or queue purged.
 * @retval -EAGAIN Waiting period timed out.
 */
int ctrl_events_put(struct event_t *event);

/**@brief  Get event from k_msgq
 *
 * @param  my_event  Event to get from the queue
 * @param  timeout   Time to wait for event. Can be K_FOREVER
 *		     K_NO_WAIT or a specific time using K_MSEC.
 *
 * @retval 0 Event received.
 * @retval -ENOMSG Returned without waiting.
 * @retval -EAGAIN Waiting period timed out.
 */
int ctrl_events_get(struct event_t *my_event, k_timeout_t timeout);

#endif /* _CTRL_EVENTS_H_ */

/** @} */ /* End addtogroup */
