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

#include "data_fifo.h"

#include <zephyr.h>

#include "macros_common.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(data_fifo, CONFIG_LOG_DEFAULT_LEVEL);

static struct k_spinlock lock;

/** @brief Checks that the elements in the msgq and slab are legal.
 * I.e. the number of msgq elements cannot be more than mem blocks used.
 */
static int msgq_slab_legal_used_elements(data_fifo_t *data_fifo, uint32_t *msgq_num_used_in,
					 uint32_t *slab_blocks_num_used_in)
{
	/* Lock so msgq and slab reads are in sync */
	k_spinlock_key_t key = k_spin_lock(&lock);

	uint32_t msgq_num_used = k_msgq_num_used_get(&data_fifo->msgq);
	uint32_t slab_blocks_num_used = k_mem_slab_num_used_get(&data_fifo->mem_slab);

	k_spin_unlock(&lock, key);

	if (slab_blocks_num_used < msgq_num_used) {
		LOG_ERR("Num used mgsq %d cannot be larger than used blocks %d", msgq_num_used,
			slab_blocks_num_used);
		return -EACCES;
	}

	if (msgq_num_used_in != NULL && slab_blocks_num_used_in != NULL) {
		*msgq_num_used_in = msgq_num_used;
		*slab_blocks_num_used_in = slab_blocks_num_used;
	}

	return 0;
}

int data_fifo_pointer_first_vacant_get(data_fifo_t *data_fifo, void **data, k_timeout_t timeout)
{
	__ASSERT_NO_MSG(data_fifo != NULL);
	__ASSERT_NO_MSG(data_fifo->initialized);
	int ret;

	ret = k_mem_slab_alloc(&data_fifo->mem_slab, data, timeout);
	return ret;
}

int data_fifo_block_lock(data_fifo_t *data_fifo, void **data, size_t size)
{
	__ASSERT_NO_MSG(data_fifo != NULL);
	__ASSERT_NO_MSG(data_fifo->initialized);
	int ret;

	if (size > data_fifo->block_size_max) {
		LOG_ERR("Size %zu too big", size);
		return -ENOMEM;
	} else if (size == 0) {
		LOG_ERR("Size is zero");
		return -EINVAL;
	}

	data_fifo_msgq_t msgq_tmp;

	msgq_tmp.block_ptr = *data;
	msgq_tmp.size = size;

	/* Since num elements in the slab and msgq are equal, there
	 * must be space in the queue. if k_msg_put fails, it
	 * is fatal.
	 */
	ret = k_msgq_put(&data_fifo->msgq, &msgq_tmp, K_NO_WAIT);
	if (ret) {
		LOG_ERR("Fatal error %d from k_msgq_put", ret);
		return -ESPIPE;
	}

	return 0;
}

int data_fifo_pointer_last_filled_get(data_fifo_t *data_fifo, void **data, size_t *size,
				      k_timeout_t timeout)
{
	__ASSERT_NO_MSG(data_fifo != NULL);
	__ASSERT_NO_MSG(data_fifo->initialized);
	int ret;

	data_fifo_msgq_t msgq_tmp;

	ret = k_msgq_get(&data_fifo->msgq, &msgq_tmp, timeout);
	if (ret) {
		return ret;
	}

	*data = msgq_tmp.block_ptr;
	*size = msgq_tmp.size;
	return 0;
}

int data_fifo_block_free(data_fifo_t *data_fifo, void **data)
{
	__ASSERT_NO_MSG(data_fifo != NULL);
	__ASSERT_NO_MSG(data_fifo->initialized);

	k_mem_slab_free(&data_fifo->mem_slab, data);

	return 0;
}

int data_fifo_num_used_get(data_fifo_t *data_fifo, uint32_t *alloced_num, uint32_t *locked_num)
{
	__ASSERT_NO_MSG(data_fifo != NULL);
	__ASSERT_NO_MSG(data_fifo->initialized);
	int ret;

	uint32_t msgq_num_used = UINT32_MAX;
	uint32_t slab_blocks_num_used = UINT32_MAX;

	ret = msgq_slab_legal_used_elements(data_fifo, &msgq_num_used, &slab_blocks_num_used);
	if (ret) {
		return ret;
	}

	*locked_num = msgq_num_used;
	*alloced_num = slab_blocks_num_used;

	return ret;
}

int data_fifo_init(data_fifo_t *data_fifo)
{
	__ASSERT_NO_MSG(data_fifo != NULL);
	__ASSERT_NO_MSG(!data_fifo->initialized);
	__ASSERT_NO_MSG(data_fifo->elements_max != 0);
	__ASSERT_NO_MSG(data_fifo->block_size_max != 0);
	__ASSERT_NO_MSG((data_fifo->block_size_max % WB_UP(1)) == 0);
	int ret;

	k_msgq_init(&data_fifo->msgq, data_fifo->msgq_buffer, sizeof(data_fifo_msgq_t),
		    data_fifo->elements_max);

	ret = k_mem_slab_init(&data_fifo->mem_slab, data_fifo->slab_buffer,
			      data_fifo->block_size_max, data_fifo->elements_max);
	if (ret) {
		LOG_ERR("Slab init failed with %d\n", ret);
		return ret;
	}

	data_fifo->initialized = true;

	return ret;
}
