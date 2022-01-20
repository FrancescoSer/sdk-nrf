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

#include "audio_usb.h"

#include <zephyr.h>
#include <usb/usb_device.h>
#include <usb/class/usb_audio.h>

#include "macros_common.h"
#include "data_fifo.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(audio_usb, CONFIG_LOG_AUDIO_USB_LEVEL);

#define USB_FRAME_SIZE_STEREO \
	(((CONFIG_AUDIO_SAMPLE_RATE_HZ * CONFIG_AUDIO_CONTAINER_BITS) / (8 * 1000)) * 2)

static data_fifo_t *fifo_tx;
static data_fifo_t *fifo_rx;

NET_BUF_POOL_FIXED_DEFINE(pool_out, CONFIG_FIFO_FRAME_SPLIT_NUM, USB_FRAME_SIZE_STEREO, 8,
			  net_buf_destroy);

#if (CONFIG_STREAM_BIDIRECTIONAL)
static void data_write(const struct device *dev)
{
	int ret;

	if (fifo_tx == NULL) {
		return;
	}

	void *data_out;
	size_t data_out_size;
	struct net_buf *buf_out;

	buf_out = net_buf_alloc(&pool_out, K_NO_WAIT);

	ret = data_fifo_pointer_last_filled_get(fifo_tx, &data_out, &data_out_size, K_NO_WAIT);
	if (ret) {
		LOG_WRN("USB TX underrun");
		net_buf_unref(buf_out);
		return;
	}

	memcpy(buf_out->data, data_out, data_out_size);
	data_fifo_block_free(fifo_tx, &data_out);

	if (data_out_size == usb_audio_get_in_frame_size(dev)) {
		ret = usb_audio_send(dev, buf_out, data_out_size);
		if (ret) {
			LOG_WRN("USB TX failed, ret: %d", ret);
			net_buf_unref(buf_out);
		}

	} else {
		LOG_WRN("Wrong size write: %d", data_out_size);
	}
}
#endif /* (CONFIG_STREAM_BIDIRECTIONAL) */

static void data_received(const struct device *dev, struct net_buf *buffer, size_t size)
{
	int ret;
	void *data_in;

	if (fifo_rx == NULL) {
		/* Throwing away data */
		net_buf_unref(buffer);
		return;
	}

	if (!buffer || !size) {
		/* This should never happen */
		ERR_CHK(-EINVAL);
	}

	/* Receive data from USB */
	if (size != USB_FRAME_SIZE_STEREO) {
		LOG_WRN("Wrong length: %d", size);
		net_buf_unref(buffer);
		return;
	}

	ret = data_fifo_pointer_first_vacant_get(fifo_rx, &data_in, K_NO_WAIT);

	/* RX FIFO can fill up due to retransmissions or disconnect */
	if (ret == -ENOMEM) {
		void *temp;
		size_t temp_size;

		LOG_WRN("USB RX overrun");

		ret = data_fifo_pointer_last_filled_get(fifo_rx, &temp, &temp_size, K_NO_WAIT);
		ERR_CHK(ret);

		ret = data_fifo_block_free(fifo_rx, &temp);
		ERR_CHK(ret);

		ret = data_fifo_pointer_first_vacant_get(fifo_rx, &data_in, K_NO_WAIT);
	}

	ERR_CHK_MSG(ret, "RX failed to get block");

	memcpy(data_in, buffer->data, size);

	ret = data_fifo_block_lock(fifo_rx, &data_in, size);
	ERR_CHK_MSG(ret, "Failed to lock block");

	net_buf_unref(buffer);
}

static void feature_update(const struct device *dev, const struct usb_audio_fu_evt *evt)
{
	LOG_DBG("Control selector %d for channel %d updated", evt->cs, evt->channel);
	switch (evt->cs) {
	case USB_AUDIO_FU_MUTE_CONTROL:
		/* Fall through */
	default:
		break;
	}
}

static const struct usb_audio_ops ops = {
	.data_received_cb = data_received,
	.feature_update_cb = feature_update,
#if (CONFIG_STREAM_BIDIRECTIONAL)
	.data_request_cb = data_write,
#endif /* (CONFIG_STREAM_BIDIRECTIONAL) */
};

int audio_usb_start(data_fifo_t *fifo_tx_in, data_fifo_t *fifo_rx_in)
{
	if (fifo_tx_in == NULL || fifo_rx_in == NULL) {
		return -EINVAL;
	}

	fifo_tx = fifo_tx_in;
	fifo_rx = fifo_rx_in;

	return 0;
}

void audio_usb_stop(void)
{
	fifo_tx = NULL;
	fifo_rx = NULL;
}

int audio_usb_disable(void)
{
	int ret;

	audio_usb_stop();

	ret = usb_disable();
	RET_IF_ERR_MSG(ret, "Failed to disable USB");

	return 0;
}

int audio_usb_init(void)
{
	int ret;
	const struct device *hs_dev = device_get_binding("HEADSET");

	if (!hs_dev) {
		LOG_ERR("Can not get USB Headset Device");
		return -EIO;
	}

	usb_audio_register(hs_dev, &ops);

	ret = usb_enable(NULL);
	RET_IF_ERR_MSG(ret, "Failed to enable USB");

	return 0;
}
