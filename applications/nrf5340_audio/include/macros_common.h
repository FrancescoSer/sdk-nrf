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

#ifndef _MACROS_H_
#define _MACROS_H_

#include <errno.h>

#include "error_handler.h"

/* Error check. If != 0, print err code and call _SysFatalErrorHandler in main.
 * For debug mode all LEDs are turned on in case of an error.
 */

#if defined(CONFIG_DEBUG)
#define PRINT_AND_OOPS(code) \
	do { \
		LOG_ERR("ERR_CHK Err_code: [%d] @ line: %d\t", code, __LINE__); \
		k_oops(); \
	} while (0)
#else
#define PRINT_AND_OOPS(code) \
	do { \
		LOG_ERR("ERR_CHK Err_code: [%s] @ line: %d\t", STRINGIFY(code), __LINE__); \
		k_oops(); \
	} while (0)
#endif /* (CONFIG_DEBUG) */

#define ERR_CHK(err_code) \
	do { \
		if (err_code) { \
			PRINT_AND_OOPS(err_code); \
		} \
	} while (0)

#define ERR_CHK_MSG(err_code, msg) \
	do { \
		if (err_code) { \
			LOG_ERR("%s", msg); \
			PRINT_AND_OOPS(err_code); \
		} \
	} while (0)

#define ERR_ASSERT(assert_val) \
	do { \
		if (!(assert_val)) { \
			PRINT_AND_OOPS(assert_val); \
		} \
	} while (0)

#define ERR_ASSERT_MSG(assert_val, msg) \
	do { \
		if (!(assert_val)) { \
			LOG_ERR("%s", msg); \
			PRINT_AND_OOPS(assert_val); \
		} \
	} while (0)

#define RET_IF_ERR(err_code) \
	do { \
		if (err_code) { \
			return err_code; \
		} \
	} while (0)

#define RET_IF_ERR_MSG(err_code, msg) \
	do { \
		if (err_code) { \
			LOG_ERR("%s", msg); \
			return err_code; \
		} \
	} while (0)

#if (defined(CONFIG_INIT_STACKS) && defined(CONFIG_THREAD_ANALYZER))

#define STACK_USAGE_PRINT(thread_name, p_thread) \
	do { \
		static uint64_t thread_ts; \
		size_t unused_space_in_thread_bytes; \
		if (k_uptime_get() - thread_ts > CONFIG_PRINT_STACK_USAGE_MS) { \
			k_thread_stack_space_get(p_thread, &unused_space_in_thread_bytes); \
			thread_ts = k_uptime_get(); \
			LOG_DBG("Unused space in %s thread: %d bytes", thread_name, \
				unused_space_in_thread_bytes); \
		} \
	} while (0)
#else
#define STACK_USAGE_PRINT(thread_name, p_stack)
#endif /* (defined(CONFIG_INIT_STACKS) && defined(CONFIG_THREAD_ANALYZER)) */

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif /* MIN */

#define COLOR_BLACK   "\x1B[0;30m"
#define COLOR_RED     "\x1B[0;31m"
#define COLOR_GREEN   "\x1B[0;32m"
#define COLOR_YELLOW  "\x1B[0;33m"
#define COLOR_BLUE    "\x1B[0;34m"
#define COLOR_MAGENTA "\x1B[0;35m"
#define COLOR_CYAN    "\x1B[0;36m"
#define COLOR_WHITE   "\x1B[0;37m"

#define COLOR_RESET "\x1b[0m"

#define BIT_SET(REG, BIT)   ((REG) |= (BIT))
#define BIT_CLEAR(REG, BIT) ((REG) &= ~(BIT))

#endif /* _MACROS_H_ */
