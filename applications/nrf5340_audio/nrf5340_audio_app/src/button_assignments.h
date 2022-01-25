/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/** @file
 *  @brief Button assignments
 *
 * Button mappings are listed here.
 *
 */

#ifndef _BUTTON_ASSIGNMENTS_H_
#define _BUTTON_ASSIGNMENTS_H_

#include <drivers/gpio.h>

/**@brief List of buttons and associated metadata
 * @details X(_name, _dts_alias)
 */
#define BUTTONS_LIST \
	X(BUTTON_VOLUME_DOWN, sw0) \
	X(BUTTON_VOLUME_UP, sw1) \
	X(BUTTON_PLAY_PAUSE, sw2) \
	X(BUTTON_TEST_TONE, sw3) \
	X(BUTTON_MUTE, sw4)

enum button_pin_names {
#define X(_name, _dts_alias) _name = DT_GPIO_PIN(DT_ALIAS(_dts_alias), gpios),
	BUTTONS_LIST
#undef X
};

#endif /* _BUTTON_ASSIGNMENTS_H_ */
