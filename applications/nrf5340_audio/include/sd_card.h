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

#ifndef _SD_CARD_H_
#define _SD_CARD_H_

#include <stddef.h>

/**@brief Print out the contents under SD card root path
 *
 * @param[in] path	Path of the folder which going to list
 *			If assigned path is null, then listing the contents under root
 *			If assigned path doesn't exist, an error will be returned
 *
 * @return	0 on success.
 *              -ENODEV SD init failed. SD card likely not inserted
 *		Otherwise, error from underlying drivers
 */
int sd_card_list_files(char *path);

/**@brief Write data from buffer into the file
 *
 * @note If the file already exists, data will be appended to the end of the file.
 *
 * @param[in] filename	Name of the target file for writing, the default location is the
 *			root directoy of SD card, accept absolute path under root of SD card
 * @param[in] data	Data which going to be written into the file
 * @param[in,out] size	Pointer to the number of bytes which is going to be written
 *			The actual written size will be returned
 *
 * @return	0 on success.
 *              -ENODEV SD init failed. SD card likely not inserted
 *		Otherwise, error from underlying drivers
 */
int sd_card_write(char const *const filename, char const *const data, size_t *size);

/**@brief Read data from file into the buffer
 *
 * @param[in] filename	Name of the target file for reading, the default location is the
 *			root directoy of SD card, accept absolute path under root of SD card
 * @param[in] data	The buffer which will be filled by read file contents
 * @param[in,out] size	Pointer to the number of bytes which wait to be read from the file
 *			The actual read size will be returned
 *			If the actual read size is 0, there will be a warning message which
 *			indicates the file is empty
 * @return	0 on success.
 *              -ENODEV SD init failed. SD card likely not inserted
 *		Otherwise, error from underlying drivers
 */
int sd_card_read(char const *const filename, char *const data, size_t *size);

/**@brief  Initialize the SD card interface and print out SD card details.
 *
 * @retval	0 on success
 *              -ENODEV SD init failed. SD card likely not inserted
 *		Otherwise, error from underlying drivers
 */
int sd_card_init(void);

#endif /* _SD_CARD_H_ */
