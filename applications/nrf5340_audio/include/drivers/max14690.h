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

#ifndef _MAX_14690_
#define _MAX_14690_

#include <zephyr.h>
#include <device.h>

/* Buck inductor value */
#define PMIC_BUCK_CFG_IND_OFFS (0)
enum pmic_buck_cfg_ind {
	PMIC_BUCK_CFG_IND_2_2_UH,
	PMIC_BUCK_CFG_IND_4_7_UH,
	PMIC_BUCK_CFG_IND_LEN,
};

#define PMIC_BUCK_CFG_MODE_OFFS (1)
enum pmic_buck_cfg_mode {
	PMIC_BUCK_CFG_MODE_BURST,
	PMIC_BUCK_CFG_MODE_FORCED_PWM,
	PMIC_BUCK_CFG_MODE_EN_WHEN_MPC0_HIGH,
	PMIC_BUCK_CFG_MODE_EN_WHEN_MPC1_HIGH,
	PMIC_BUCK_CFG_MODE_LEN,
};

#define PMIC_BUCK_CFG_EN_OFFS (3)
enum pmic_buck_cfg_en {
	PMIC_BUCK_CFG_EN_DISABLED,
	PMIC_BUCK_CFG_EN_ENABLED,
	PMIC_BUCK_CFG_EN_WHEN_MPC0_HIGH,
	PMIC_BUCK_CFG_EN_WHEN_MPC1_HIGH,
	PMIC_BUCK_CFG_EN_LEN,
};

/* Buck 1 voltage settings in millivolts */
#define PMIC_BUCK_1_VOLTAGE_STEP_MV 25
#define PMIC_BUCK_1_VOLTAGE_MIN_MV  800
#define PMIC_BUCK_1_VOLTAGE_MAX_MV  1800

/* Buck 2 voltage settings in millivolts */
#define PMIC_BUCK_2_VOLTAGE_STEP_MV 50
#define PMIC_BUCK_2_VOLTAGE_MIN_MV  1500
#define PMIC_BUCK_2_VOLTAGE_MAX_MV  3300

#define PMIC_LDO_CFG_MODE_OFFS (0)
enum pmic_ldo_cfg_mode {
	PMIC_LDO_CFG_MODE_NORMAL,
	PMIC_LDO_CFG_MODE_LOAD_SWITCH,
	PMIC_LDO_CFG_MODE_LEN,
};

#define PMIC_LDO_CFG_EN_OFFS (1)
enum pmic_ldo_cfg_en {
	PMIC_LDO_CFG_EN_DISABLED,
	PMIC_LDO_CFG_EN_ENABLED,
	PMIC_LDO_CFG_EN_WHEN_MPC0_HIGH,
	PMIC_LDO_CFG_EN_WHEN_MPC1_HIGH,
	PMIC_LDO_CFG_EN_LEN,
};

#define PMIC_LDO_CFG_DSC_OFFS (3)
enum pmic_ldo_cfg_dsc {
	PMIC_LDO_CFG_DSC_HARD_RESET_ONLY,
	PMIC_LDO_CFG_DSC_HARD_RESET_AND_ENABLE,
	PMIC_LDO_CFG_DSC_LEN,
};

#define PMIC_PWR_CFG_STAY_ON_OFFS (0)
enum pmic_pwr_cfg_stay_on {
	PMIC_PWR_CFG_STAY_ON_DISABLED,
	PMIC_PWR_CFG_STAY_ON_ENABLED,
	PMIC_PWR_CFG_STAY_ON_LEN,
};

#define PMIC_PWR_CFG_PFN_OFFS (7)
enum pmic_pwr_cfg_pfn {
	PMIC_PWR_CFG_PFN_DISABLED,
	PMIC_PWR_CFG_PFN_ENABLED,
	PMIC_PWR_CFG_PFN_LEN,
};

#define PMIC_MTN_CHG_TMR_OFFS (4)
enum pmic_mtn_chg_tmr {
	PMIC_MTN_CHG_TMR_0_MIN,
	PMIC_MTN_CHG_TMR_15_MIN,
	PMIC_MTN_CHG_TMR_30_MIN,
	PMIC_MTN_CHG_TMR_60_MIN,
	PMIC_MTN_CHG_TMR_LEN,
};

#define PMIC_FAST_CHG_TMR_OFFS (2)
enum pmic_fast_chg_tmr {
	PMIC_FAST_CHG_TMR_75_MIN,
	PMIC_FAST_CHG_TMR_150_MIN,
	PMIC_FAST_CHG_TMR_300_MIN,
	PMIC_FAST_CHG_TMR_600_MIN,
	PMIC_FAST_CHG_TMR_LEN,
};

#define PMIC_PRE_CHG_TMR_OFFS (0)
enum pmic_pre_chg_tmr {
	PMIC_PRE_CHG_TMR_30_MIN,
	PMIC_PRE_CHG_TMR_60_MIN,
	PMIC_PRE_CHG_TMR_120_MIN,
	PMIC_PRE_CHG_TMR_240_MIN,
	PMIC_PRE_CHG_TMR_LEN,
};

#define PMIC_THRM_CFG_OFFS (0)
enum pmic_thrm_cfg {
	PMIC_THRM_CFG_THERM_DISABLED_JEITA_DISABLED,
	PMIC_THRM_CFG_THERM_ENABLED_JEITA_DISABLED,
	PMIC_THRM_CFG_DO_NOT_USE,
	PMIC_THRM_CFG_THERM_ENABLED_JEITA_ENABLED,
	PMIC_THRM_CFG_LEN,
};

#define PMIC_MON_RATIO_CFG_OFFS (4)
enum pmic_mon_ratio_cfg {
	PMIC_MON_RATIO_4_1,
	PMIC_MON_RATIO_3_1,
	PMIC_MON_RATIO_2_1,
	PMIC_MON_RATIO_1_1,
	PMIC_MON_RATIO_LEN,
};

#define PMIC_MON_OFF_MODE_CFG_OFFS (3)
enum pmic_mon_off_mode_cfg {
	PMIC_MON_OFF_MODE_PULL_DOWN_100K,
	PMIC_MON_OFF_MODE_HIZ,
	PMIC_MON_OFF_MODE_LEN,
};

#define PMIC_MON_CTR_OFFS (0)
enum pmic_mon_ctr_cfg {
	PMIC_MON_CTR_NOT_CONNECTED,
	PMIC_MON_CTR_BATT,
	PMIC_MON_CTR_SYS,
	PMIC_MON_CTR_BUCK_1,
	PMIC_MON_CTR_BUCK_2,
	PMIC_MON_CTR_LDO_1,
	PMIC_MON_CTR_LDO_2,
	PMIC_MON_CTR_LDO_3,
	PMIC_MON_CTR_LEN,
};

/**@brief Configures the inductor value for buck 1.
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_buck_1_cfg_ind_cfg(enum pmic_buck_cfg_ind cfg);

/**@brief Configures the mode for buck 1.
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_buck_1_cfg_mode_cfg(enum pmic_buck_cfg_mode cfg);

/**@brief Configures the enable bits for buck 1.
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_buck_1_cfg_enable_cfg(enum pmic_buck_cfg_en cfg);

/**@brief Configures the voltage for buck 1.
 *
 * @param [in]  mv      The desired voltage in millivolts
 *
 * @retval 0            If configuration was successful.
 *
 * @note This register can only be changed when buck1 is disabled.
 */
int max14690_buck_1_voltage_cfg(uint16_t mv);

/**@brief Configures the inductor value for buck 2.
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_buck_2_cfg_ind_cfg(enum pmic_buck_cfg_ind cfg);

/**@brief Configures the mode for buck 2.
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_buck_2_cfg_mode_cfg(enum pmic_buck_cfg_mode cfg);

/**@brief Configures the enable bits for buck 2.
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_buck_2_cfg_enable_cfg(enum pmic_buck_cfg_en cfg);

/**@brief Configures the voltage for buck 2.
 *
 * @param [in]  mv      The desired voltage in millivolts
 *
 * @retval 0            If configuration was successful.
 *
 * @note This register can only be changed when buck2 is disabled.
 */
int max14690_buck_2_voltage_cfg(uint16_t mv);

/**@brief Configures the mode for LDO 1.
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_ldo_1_cfg_mode_cfg(enum pmic_ldo_cfg_mode cfg);

/**@brief Configures the enable for LDO 1.
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_ldo_1_cfg_enable_cfg(enum pmic_ldo_cfg_en cfg);

/**@brief Configures the active discharge control for LDO 1.
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_ldo_1_cfg_dsc_cfg(enum pmic_ldo_cfg_dsc cfg);

/**@brief Configures the mode for LDO 2.
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_ldo_2_cfg_mode_cfg(enum pmic_ldo_cfg_mode cfg);

/**@brief Configures the enable for LDO 2.
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_ldo_2_cfg_enable_cfg(enum pmic_ldo_cfg_en cfg);

/**@brief Configures the active discharge control for LDO 2.
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_ldo_2_cfg_dsc_cfg(enum pmic_ldo_cfg_dsc cfg);

/**@brief Configures the mode for LDO 3.
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_ldo_3_cfg_mode_cfg(enum pmic_ldo_cfg_mode cfg);

/**@brief Configures the enable for LDO 3.
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_ldo_3_cfg_enable_cfg(enum pmic_ldo_cfg_en cfg);

/**@brief Configures the active discharge control for LDO 3.
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_ldo_3_cfg_dsc_cfg(enum pmic_ldo_cfg_dsc cfg);

/**@brief Configures the StayOn bit.
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_boot_cfg_stay_on_cfg(enum pmic_pwr_cfg_stay_on cfg);

/**@brief Configures the automatic pull-up/pull-down.
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_boot_cfg_pfn_cfg(enum pmic_pwr_cfg_pfn cfg);

/**@brief Configures the maintain charge timer
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_mtn_chg_tmr_cfg(enum pmic_mtn_chg_tmr cfg);

/**@brief Configures the fast-charge timer
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_fast_chg_tmr_cfg(enum pmic_fast_chg_tmr cfg);

/**@brief Configures the precharge timer
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_pre_chg_tmr_cfg(enum pmic_pre_chg_tmr cfg);

/**@brief Configures the thermal monitoring
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_pmic_thrm_cfg(enum pmic_thrm_cfg cfg);

/**@brief Configures the MON Resistive Partition Selector
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_pmic_mon_ratio_cfg(enum pmic_mon_ratio_cfg cfg);

/**@brief Configures the MON Off mode condition
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_pmic_mon_off_mode_cfg(enum pmic_mon_off_mode_cfg cfg);

/**@brief Configures the MON Pin source selection
 *
 * @param [in]  cfg     Enum for configuring the value.
 *
 * @retval 0            If configuration was successful.
 *
 * @note This function only changes the affected bits and leaves
 * the rest unaltered. After writing, the register is read back
 * to verify the contents.
 */
int max14690_pmic_mon_ctr_cfg(enum pmic_mon_ctr_cfg cfg);

/**@brief Sends Power-Off Command to PMIC
 *
 * @retval 0            If successful.
 *
 * @note This function may power off the controlling device, and hence,
 * a return may not be expected at all. Note that this command may work
 * differently based on the PwrRstCfg and if the unit is being charged.
 */
int max14690_pwr_off(void);

int max14690_init(const struct device *dev);

#endif /* _MAX_14690_ */
