/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "board_version.h"

#include <zephyr.h>
#include <drivers/adc.h>
#include <drivers/gpio.h>
#include <stdlib.h>
#include <nrfx_saadc.h>

#include "board.h"
#include "macros_common.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(board_version, CONFIG_LOG_BOARD_VERSION_LEVEL);

#define ADC_1ST_CHANNEL_ID 0
#define ADC_RESOLUTION_BITS 12
/* We allow the ADC register value to deviate by N points in either direction */
#define BOARD_VERSION_TOLERANCE 70
#define ADC_ACQ_TIME_US 40
#define VOLTAGE_STABILIZE_TIME_US 5

static int16_t sample_buffer;
static const struct device *adc_dev;
static const struct device *gpio_dev;

static const struct adc_channel_cfg m_channel_cfg = {
	.gain = ADC_GAIN_1_4,
	.reference = ADC_REF_VDD_1_4,
	.acquisition_time = ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, ADC_ACQ_TIME_US),
	.channel_id = ADC_1ST_CHANNEL_ID,
	.differential = 0,
#if defined(CONFIG_ADC_CONFIGURABLE_INPUTS)
	.input_positive = NRF_SAADC_INPUT_AIN6,
#endif /* defined(CONFIG_ADC_CONFIGURABLE_INPUTS) */
};

static const struct adc_sequence sequence = {
	.channels = BIT(ADC_1ST_CHANNEL_ID),
	.buffer = (void *)&sample_buffer,
	.buffer_size = sizeof(sample_buffer),
	.resolution = ADC_RESOLUTION_BITS,
	.oversampling = NRF_SAADC_OVERSAMPLE_256X,
};

/* @brief Enable board version voltage divider and trigger ADC read */
static int divider_value_get(void)
{
	int ret;

	ret = gpio_pin_set(gpio_dev, DT_GPIO_PIN(DT_NODELABEL(board_id_en_out), gpios), 1);
	RET_IF_ERR(ret);

	/* Wait for voltage to stabilize */
	k_busy_wait(VOLTAGE_STABILIZE_TIME_US);

	ret = adc_read(adc_dev, &sequence);
	RET_IF_ERR(ret);

	ret = gpio_pin_set(gpio_dev, DT_GPIO_PIN(DT_NODELABEL(board_id_en_out), gpios), 0);
	RET_IF_ERR(ret);

	return 0;
}

/**@brief Traverse all defined versions and get the one with the
 * most similar value. Check tolerances.
 */
static int version_search(int reg_value, uint32_t tolerance, struct board_version *board_rev)
{
	uint32_t diff;
	uint32_t smallest_diff = UINT_MAX;
	uint8_t smallest_diff_idx = UCHAR_MAX;

	for (uint8_t i = 0; i < (uint8_t)ARRAY_SIZE(BOARD_VERSION_ARR); i++) {
		diff = abs(BOARD_VERSION_ARR[i].adc_reg_val - reg_value);

		if (diff < smallest_diff) {
			smallest_diff = diff;
			smallest_diff_idx = i;
		}
	}

	if (smallest_diff >= tolerance) {
		LOG_ERR("Board ver search failed. ADC reg read: %d", reg_value);
		return -ESPIPE; /* No valid board_rev found */
	}

	*board_rev = BOARD_VERSION_ARR[smallest_diff_idx];
	LOG_DBG("Board ver search OK!. ADC reg val: %d", reg_value);
	return 0;
}

/* @brief Internal init routine */
static int board_version_init(void)
{
	int ret;
	static bool initialized;

	if (initialized) {
		return 0;
	}

	gpio_dev = device_get_binding("GPIO_0");
	if (gpio_dev == NULL) {
		return -ENXIO;
	}

	ret = gpio_pin_configure(gpio_dev, DT_GPIO_PIN(DT_NODELABEL(board_id_en_out), gpios),
				 GPIO_OUTPUT_LOW);
	RET_IF_ERR(ret);

	adc_dev = device_get_binding("ADC_0");
	if (adc_dev == NULL) {
		LOG_ERR("Failed to get ADC");
		return -ENXIO;
	}

	ret = adc_channel_setup(adc_dev, &m_channel_cfg);
	RET_IF_ERR(ret);

	initialized = true;
	return 0;
}

int board_version_get(struct board_version *board_rev)
{
	int ret;

	ret = board_version_init();
	RET_IF_ERR(ret);

	ret = divider_value_get();
	RET_IF_ERR(ret);

	ret = version_search(sample_buffer, BOARD_VERSION_TOLERANCE, board_rev);
	RET_IF_ERR(ret);

	return 0;
}

int board_version_valid_check(void)
{
	int ret;
	struct board_version board_rev;

	ret = board_version_get(&board_rev);
	RET_IF_ERR_MSG(ret, "Unable to get any board version");

	if (BOARD_VERSION_VALID_MSK & (board_rev.mask)) {
		LOG_INF(COLOR_GREEN "Compatible board/HW version found: %s" COLOR_RESET,
			board_rev.name);
	} else {
		LOG_ERR("Invalid board found, rev: %s Valid mask: 0x%x valid mask: 0x%lx",
			board_rev.name, board_rev.mask, BOARD_VERSION_VALID_MSK);
		return -EPERM;
	}

	return 0;
}
