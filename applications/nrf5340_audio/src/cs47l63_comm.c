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

#include "cs47l63_comm.h"

#include <zephyr.h>
#include <drivers/spi.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <kernel.h>
#include <drivers/gpio.h>

#include "macros_common.h"
#include "bsp_driver_if.h"
#include "cs47l63.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(CS47L63, CONFIG_LOG_CS47L63_LEVEL);

#define DEFAULT_DEVID 0x47A63
#define BUS_TYPE_SPI  1
#define PAD_LEN	      4 /* Four bytes padding after address */
/* Delay the processing thread to allow interrupts to settle after boot */
#define CS47L63_PROCESS_THREAD_DELAY_MS 10

#define CS47L63_INT_PIN	  DT_GPIO_PIN(DT_NODELABEL(dsp_irq_in), gpios)
#define CS47L63_GPIO_PIN  DT_GPIO_PIN(DT_NODELABEL(dsp_gpio_in), gpios)
#define CS47L63_RESET_PIN DT_GPIO_PIN(DT_NODELABEL(dsp_reset_out), gpios)

const static struct device *cirrus_dev;
const static struct device *gpio_dev;

static bsp_callback_t bsp_callback;
static void *bsp_callback_arg;

static struct gpio_callback gpio_cb;

static struct k_thread cs47l63_data;
K_THREAD_STACK_DEFINE(cs47l63_stack, CONFIG_CS47L63_STACK_SIZE);

K_SEM_DEFINE(sem_cs47l63, 0, 1);

/* Free the CS pin and release the SPI device after a transaction */
#define SPI_OPER_POST_FREE \
	(SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB | SPI_WORD_SET(8) | SPI_LINES_SINGLE)

/* Hold the CS pin and do not release the SPI device after a transaction */
#define SPI_OPER_POST_HOLD \
	(SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB | SPI_HOLD_ON_CS | SPI_LOCK_ON | SPI_WORD_SET(8) | \
	 SPI_LINES_SINGLE)

#define SPI_FREQUENCY 8000000

static struct spi_config config = { .frequency = SPI_FREQUENCY,
				    .operation = SPI_OPER_POST_FREE,
				    .slave = 0 };

struct k_mutex cirrus_reg_oper_mutex;

static void notification_callback(uint32_t event_flags, void *arg)
{
	LOG_DBG("Notification from CS47L63, flags: %d", event_flags);
}

/* Locks the mutex and holds the CS pin
 * for consecutive transactions
 */
static void spi_mutex_lock(void)
{
	int ret;

	ret = k_mutex_lock(&cirrus_reg_oper_mutex, K_FOREVER);
	ERR_CHK(ret);

	/* If operation mode set to HOLD or the SPI_LOCK_ON is set when
	 * taking the mutex something is wrong
	 */
	if ((config.operation & SPI_HOLD_ON_CS) || (config.operation & SPI_LOCK_ON)) {
		ERR_CHK_MSG(-EPERM,
			    "SPI_HOLD_ON_CS and SPI_LOCK_ON must be freed before releasing mutex");
	}
}

/* Unlocks mutex and CS pin */
static void spi_mutex_unlock(void)
{
	/* If operation mode still set to HOLD or
	 * the SPI_LOCK_ON is still set when releasing the mutex
	 * something is wrong
	 */
	if ((config.operation & SPI_HOLD_ON_CS) || (config.operation & SPI_LOCK_ON)) {
		ERR_CHK_MSG(-EPERM,
			    "SPI_HOLD_ON_CS and SPI_LOCK_ON must be freed before releasing mutex");
	}

	k_mutex_unlock(&cirrus_reg_oper_mutex);
}

/* Pin interrupt handler for CS47L63 */
static void cs47l63_comm_pin_int_handler(const struct device *gpio_port, struct gpio_callback *cb,
					 uint32_t pins)
{
	if (bsp_callback == NULL) {
		ERR_CHK_MSG(-ENODEV, "No callback registered");
	}

	if (pins == BIT(CS47L63_INT_PIN)) {
		bsp_callback(BSP_STATUS_OK, bsp_callback_arg);
		k_sem_give(&sem_cs47l63);
	}
}

static uint32_t cs47l63_comm_reg_read(uint32_t bsp_dev_id, uint8_t *addr_buffer,
				      uint32_t addr_length, uint8_t *data_buffer,
				      uint32_t data_length, uint32_t pad_len)
{
	if (!cirrus_dev) {
		LOG_ERR("SPI driver was not found!");
		return BSP_STATUS_FAIL;
	}

	if (pad_len != PAD_LEN) {
		LOG_ERR("Trying to pad more than 4 bytes: %d", pad_len);
		return BSP_STATUS_FAIL;
	}

	int ret;

	uint8_t pad_buffer[PAD_LEN] = { 0 };

	struct spi_buf_set rx;
	struct spi_buf rx_buf[] = { { .buf = addr_buffer, .len = addr_length },
				    { .buf = pad_buffer, .len = pad_len },
				    { .buf = data_buffer, .len = data_length } };

	rx.buffers = rx_buf;
	rx.count = 3;

	spi_mutex_lock();
	config.operation = SPI_OPER_POST_FREE;

	ret = spi_transceive(cirrus_dev, &config, &rx, &rx);
	ERR_CHK(ret);

	spi_mutex_unlock();

	return BSP_STATUS_OK;
}

static uint32_t cs47l63_comm_reg_write(uint32_t bsp_dev_id, uint8_t *addr_buffer,
				       uint32_t addr_length, uint8_t *data_buffer,
				       uint32_t data_length, uint32_t pad_len)
{
	if (!cirrus_dev) {
		LOG_ERR("SPI driver was not found!");
		return BSP_STATUS_FAIL;
	}

	if (pad_len != PAD_LEN) {
		LOG_ERR("Trying to pad more than 4 bytes: %d", pad_len);
		return BSP_STATUS_FAIL;
	}

	int ret;

	uint8_t pad_buffer[PAD_LEN] = { 0 };

	struct spi_buf_set tx;
	struct spi_buf tx_buf[] = { { .buf = addr_buffer, .len = addr_length },
				    { .buf = pad_buffer, .len = pad_len },
				    { .buf = data_buffer, .len = data_length } };

	tx.buffers = tx_buf;
	tx.count = 3;

	spi_mutex_lock();
	config.operation = SPI_OPER_POST_FREE;

	ret = spi_write(cirrus_dev, &config, &tx);
	if (ret) {
		LOG_ERR("SPI failed to write: %d", ret);
		return BSP_STATUS_FAIL;
	}

	spi_mutex_unlock();

	return BSP_STATUS_OK;
}

static uint32_t cs47l63_comm_gpio_set(uint32_t gpio_id, uint8_t gpio_state)
{
	int ret;

	if (!gpio_dev) {
		LOG_ERR("Failed to get gpio_dev");
		return BSP_STATUS_FAIL;
	}

	ret = gpio_pin_set(gpio_dev, gpio_id, gpio_state);

	if (ret) {
		LOG_ERR("Failed to set gpio state, ret: %d", ret);
		return BSP_STATUS_FAIL;
	}

	return BSP_STATUS_OK;
}

/* Register callback for pin interrupt from CS47L63 */
static uint32_t cs47l63_comm_gpio_cb_register(uint32_t gpio_id, bsp_callback_t cb, void *cb_arg)
{
	int ret;

	bsp_callback = cb;
	bsp_callback_arg = cb_arg;

	gpio_init_callback(&gpio_cb, cs47l63_comm_pin_int_handler, BIT(gpio_id));

	ret = gpio_add_callback(gpio_dev, &gpio_cb);
	if (ret) {
		return BSP_STATUS_FAIL;
	}

	ret = gpio_pin_interrupt_configure(gpio_dev, gpio_id, GPIO_INT_EDGE_TO_INACTIVE);
	if (ret) {
		return BSP_STATUS_FAIL;
	}

	return BSP_STATUS_OK;
}

static uint32_t cs47l63_comm_timer_set(uint32_t duration_ms, bsp_callback_t cb, void *cb_arg)
{
	if (cb != NULL || cb_arg != NULL) {
		LOG_ERR("Timer with callback not supported");
		return BSP_STATUS_FAIL;
	}

	k_sleep(K_MSEC(duration_ms));

	return BSP_STATUS_OK;
}

static uint32_t cs47l63_comm_set_supply(uint32_t supply_id, uint8_t supply_state)
{
	LOG_ERR("Tried to set supply, not supported");
	return BSP_STATUS_FAIL;
}

static uint32_t cs47l63_comm_i2c_reset(uint32_t bsp_dev_id, bool *was_i2c_busy)
{
	LOG_ERR("Tried to reset I2C, not supported");
	return BSP_STATUS_FAIL;
}

static uint32_t cs47l63_comm_i2c_read_repeated_start(uint32_t bsp_dev_id, uint8_t *write_buffer,
						     uint32_t write_length, uint8_t *read_buffer,
						     uint32_t read_length, bsp_callback_t cb,
						     void *cb_arg)
{
	LOG_ERR("Tried to read repeated start I2C, not supported");
	return BSP_STATUS_FAIL;
}

static uint32_t cs47l63_comm_i2c_write(uint32_t bsp_dev_id, uint8_t *write_buffer,
				       uint32_t write_length, bsp_callback_t cb, void *cb_arg)
{
	LOG_ERR("Tried writing to I2C, not supported");
	return BSP_STATUS_FAIL;
}

static uint32_t cs47l63_comm_i2c_db_write(uint32_t bsp_dev_id, uint8_t *write_buffer_0,
					  uint32_t write_length_0, uint8_t *write_buffer_1,
					  uint32_t write_length_1, bsp_callback_t cb, void *cb_arg)
{
	LOG_ERR("Tried to write double buffered I2C, not supported");
	return BSP_STATUS_FAIL;
}

static uint32_t cs47l63_comm_enable_irq(void)
{
	LOG_ERR("Tried to enable irq, not supported");
	return BSP_STATUS_FAIL;
}

static uint32_t cs47l63_comm_disable_irq(void)
{
	LOG_ERR("Tried to disable irq, not supported");
	return BSP_STATUS_FAIL;
}

static uint32_t cs47l63_comm_spi_throttle_speed(uint32_t speed_hz)
{
	LOG_ERR("Tried to throttle SPI speed, not supported");
	return BSP_STATUS_FAIL;
}

static uint32_t cs47l63_comm_spi_restore_speed(void)
{
	LOG_ERR("Tried to restore SPI speed, not supported");
	return BSP_STATUS_FAIL;
}

/* Thread to process events from CS47L63 */
static void cs47l63_comm_thread(void *cs47l63_driver, void *dummy2, void *dummy3)
{
	int ret;

	while (1) {
		k_sem_take(&sem_cs47l63, K_FOREVER);
		ret = cs47l63_process((cs47l63_t *)cs47l63_driver);
		if (ret) {
			LOG_ERR("CS47L63 failed to process event");
		}
	}
}

static cs47l63_bsp_config_t bsp_config = { .bsp_dev_id = 1,
					   .bsp_reset_gpio_id = CS47L63_RESET_PIN,
					   .bsp_int_gpio_id = CS47L63_INT_PIN,
					   .bus_type = BUS_TYPE_SPI,
					   .notification_cb = &notification_callback,
					   .notification_cb_arg = NULL };

int cs47l63_comm_init(cs47l63_t *cs47l63_driver)
{
	int ret;
	uint32_t device_id;

	cs47l63_config_t cs47l63_config;

	memset(&cs47l63_config, 0, sizeof(cs47l63_config_t));

	k_mutex_init(&cirrus_reg_oper_mutex);

	/* Configure SPI pins */
	cirrus_dev = device_get_binding("SPI_4");

	if (!cirrus_dev) {
		LOG_ERR("SPI driver was not found!");
		return -ENXIO;
	}

	static struct spi_cs_control spi_cs_ctrl = { .gpio_pin = DT_SPI_DEV_CS_GPIOS_PIN(
							     DT_NODELABEL(cs47l63)),
						     .delay = 0,
						     .gpio_dt_flags = GPIO_ACTIVE_LOW };

	spi_cs_ctrl.gpio_dev = device_get_binding("GPIO_0");

	if (!spi_cs_ctrl.gpio_dev) {
		LOG_ERR("GPIO device not found!");
		return -ENXIO;
	}

	config.cs = &spi_cs_ctrl;

	/* Configure the rest of the pins for CS47L63 */
	gpio_dev = device_get_binding("GPIO_0");

	if (!gpio_dev) {
		LOG_ERR("GPIO device not found!");
		return -ENXIO;
	}

	ret = gpio_pin_configure(gpio_dev, CS47L63_GPIO_PIN, GPIO_INPUT | GPIO_PULL_UP);
	RET_IF_ERR(ret);

	ret = gpio_pin_configure(gpio_dev, CS47L63_INT_PIN, GPIO_INPUT | GPIO_PULL_UP);
	RET_IF_ERR(ret);

	ret = gpio_pin_configure(gpio_dev, CS47L63_RESET_PIN, GPIO_OUTPUT_HIGH);
	RET_IF_ERR(ret);

	/* Start thread to handle events from CS47L63 */
	(void)k_thread_create(&cs47l63_data, cs47l63_stack, CONFIG_CS47L63_STACK_SIZE,
			      (k_thread_entry_t)cs47l63_comm_thread, (void *)cs47l63_driver, NULL,
			      NULL, K_PRIO_PREEMPT(CONFIG_CS47L63_THREAD_PRIO), 0,
			      K_MSEC(CS47L63_PROCESS_THREAD_DELAY_MS));
	ret = k_thread_name_set(&cs47l63_data, "CS47L63");

	/* Initialize CS47L63 drivers */
	ret = cs47l63_initialize(cs47l63_driver);

	if (ret != CS47L63_STATUS_OK) {
		LOG_ERR("Failed to initialize CS47L63");
		return -ENXIO;
	}

	cs47l63_config.bsp_config = bsp_config;

	cs47l63_config.syscfg_regs = cs47l63_syscfg_regs;
	cs47l63_config.syscfg_regs_total = CS47L63_SYSCFG_REGS_TOTAL;

	ret = cs47l63_configure(cs47l63_driver, &cs47l63_config);

	if (ret != CS47L63_STATUS_OK) {
		LOG_ERR("Failed to configure CS47L63");
		return -ENXIO;
	}

	/* Make sure we are able to communicate with CS47L63 */
	ret = cs47l63_read_reg(cs47l63_driver, CS47L63_DEVID, &device_id);
	RET_IF_ERR(ret);

	if (device_id != DEFAULT_DEVID) {
		LOG_ERR("Wrong device id: 0x%02x, should be 0x%02x", (uint32_t)device_id,
			DEFAULT_DEVID);
		return -EIO;
	}

	return 0;
}

static bsp_driver_if_t bsp_driver_if_s = { .set_gpio = &cs47l63_comm_gpio_set,
					   .register_gpio_cb = &cs47l63_comm_gpio_cb_register,
					   .set_timer = &cs47l63_comm_timer_set,
					   .spi_read = &cs47l63_comm_reg_read,
					   .spi_write = &cs47l63_comm_reg_write,

					   /* Functions not supported */
					   .set_supply = &cs47l63_comm_set_supply,
					   .i2c_read_repeated_start =
						   &cs47l63_comm_i2c_read_repeated_start,
					   .i2c_write = &cs47l63_comm_i2c_write,
					   .i2c_db_write = &cs47l63_comm_i2c_db_write,
					   .i2c_reset = &cs47l63_comm_i2c_reset,
					   .enable_irq = &cs47l63_comm_enable_irq,
					   .disable_irq = &cs47l63_comm_disable_irq,
					   .spi_throttle_speed = &cs47l63_comm_spi_throttle_speed,
					   .spi_restore_speed = &cs47l63_comm_spi_restore_speed };

bsp_driver_if_t *bsp_driver_if_g = &bsp_driver_if_s;
