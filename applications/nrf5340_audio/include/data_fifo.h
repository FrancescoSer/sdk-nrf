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

#ifndef _DATA_FIFO_H_
#define _DATA_FIFO_H_

#include <stddef.h>
#include <stdint.h>
#include <kernel.h>

/* The queue elements hold a pointer to a memory block in a slab and the
 * number of bytes written to that block.
 */
typedef struct {
	void *block_ptr;
	size_t size;
} data_fifo_msgq_t;

typedef struct {
	char *msgq_buffer;
	char *slab_buffer;
	struct k_mem_slab mem_slab;
	struct k_msgq msgq;
	uint32_t elements_max;
	size_t block_size_max;
	bool initialized;
} data_fifo_t;

#define DATA_FIFO_DEFINE(name, elements_max_in, block_size_max_in) \
	char __aligned(WB_UP(1)) \
		_msgq_buffer_##name[(elements_max_in) * sizeof(data_fifo_msgq_t)] = { 0 }; \
	char __aligned(WB_UP(1)) \
		_slab_buffer_##name[(elements_max_in) * (block_size_max_in)] = { 0 }; \
	data_fifo_t name = { .msgq_buffer = _msgq_buffer_##name, \
			     .slab_buffer = _slab_buffer_##name, \
			     .block_size_max = block_size_max_in, \
			     .elements_max = elements_max_in, \
			     .initialized = false }

/**
 * @brief Get pointer to first vacant block in slab.
 *
 * Gives pointer to the first vacant memory block in the
 * slab.
 *
 * @param data_fifo Pointer to the data_fifo structure
 * @param data Double pointer to the memory area. If this function returns with
 *	success, the caller is now able to write to this memory block. Note that
 *	the write operation must not exceeed the block size max given to
 *	DATA_FIFO_DEFINE.
 * @param timeout Non-negative waiting period to wait for operation to complete
 *	(in milliseconds). Use K_NO_WAIT to return without waiting,
 *	or K_FOREVER to wait as long as necessary.
 *
 * @retval 0 Memory allocated.
 * @retval Return values from k_mem_slab_alloc.
 */
int data_fifo_pointer_first_vacant_get(data_fifo_t *data_fifo, void **data, k_timeout_t timeout);

/**
 * @brief Confirm that memory block use has finished
 * and the block put into the message queue.
 *
 * There is no mechanism blocking this region from being written to or read from.
 * Hence, this block should not be used before it is later fetched
 * by using data_fifo_pointer_last_filled_get.
 *
 * @param data_fifo Pointer to the data_fifo structure.
 * @param data Double pointer to the memory block which has been written to.
 * @param size Number of bytes written. Must be equal to or smaller
 *		than the block size max.
 *
 * @retval 0		Block has been sumbitted to the message queue.
 * @retval -ENOMEM	size is larger than block size max.
 * @retval -EINVAL	Supplied size is zero
 * @retval -ESPIPE	Generic return if an error occurs in k_msg_put.
 *			Since data has already been added to the slab, there
 *			must be space in the message queue.
 */
int data_fifo_block_lock(data_fifo_t *data_fifo, void **data, size_t size);

/**
 * @brief Get pointer to first (oldest) filled block in slab.
 *
 * This returns a pointer to the first filled block in the
 * slab (FIFO).
 *
 * @param data_fifo Pointer to the data_fifo structure.
 * @param data Double pointer to the block. If this functions returns with
 *	success, the caller is now able to read from this memory block.
 * @param size Actual size in bytes of the stored data. Note that this may
 *	be equal to or less than the block size.
 * @param timeout Non-negative waiting period to wait for operation to complete
 *	(in milliseconds). Use K_NO_WAIT to return without waiting,
 *	or K_FOREVER to wait as long as necessary.
 *
 * @retval 0 Memory pointer retrieved.
 * @retval Return values from k_msgq_get.
 */
int data_fifo_pointer_last_filled_get(data_fifo_t *data_fifo, void **data, size_t *size,
				      k_timeout_t timeout);

/**
 * @brief Free the data block after reading.
 *
 * Read has finished in the given data block.
 *
 * @param data_fifo Pointer to the data_fifo structure.
 * @param data Double pointer to the memory area which is to be freed.
 *
 * @retval 0	Memory block is freed.
 * @retval Return values from k_mem_slab_free.
 */
int data_fifo_block_free(data_fifo_t *data_fifo, void **data);

/**
 * @brief See how many alloced and locked blocks are in the system.
 *
 * @param data_fifo Pointer to the data_fifo structure.
 * @param alloced_num Number of used blocks in the slab.
 * @param locked_num Number of used items in the message queue.
 *
 * @retval 0		Success
 * @retval -EACCES	Illegal combination of used message queue items
 *			and slabs. If an error occurs, parameters
 *			will be set to UINT32_MAX.
 */
int data_fifo_num_used_get(data_fifo_t *data_fifo, uint32_t *alloced_num, uint32_t *locked_num);

/**
 * @brief Initialise the data_fifo.
 *
 * @param data_fifo Pointer to the data_fifo structure.
 *
 * @retval 0		Success
 * @retval Return values from k_mem_slab_init.
 */
int data_fifo_init(data_fifo_t *data_fifo);

#endif /* _DATA_FIFO_H_ */
