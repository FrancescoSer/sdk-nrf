/**
 * @file cs47l63_syscfg_regs.h
 *
 * @brief Register values to be applied after CS47L63 Driver boot().
 *
 * @copyright
 * Copyright (c) Cirrus Logic 2022 All Rights Reserved, http://www.cirrus.com/
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * wisce_to_syscfg_reg_converter.py SDK version: 4.11.0 - 61ee6c0881e6270b4df6e990d502275c00aad6c9
 * Command:  ..\..\tools\wisce_script_converter\wisce_script_converter.py -i
 *           wisce_init.txt -p cs47l63 -c c_array
 *
 */

#ifndef CS47L63_SYSCFG_REGS_H
#define CS47L63_SYSCFG_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "stdint.h"
#include "regmap.h"

/******************************************************************************
 * LITERALS & CONSTANTS
 ******************************************************************************/
#define CS47L63_SYSCFG_REGS_TOTAL (30)

/******************************************************************************
 * ENUMS, STRUCTS, UNIONS, TYPEDEFS
 ******************************************************************************/

/******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/
extern uint32_t cs47l63_syscfg_regs[];

#ifdef __cplusplus
}
#endif

#endif // CS47L63_SYSCFG_REGS_H
