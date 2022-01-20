# Copyright (c) 2018 Nordic Semiconductor ASA
#
# All rights reserved.
#
# SPDX-License-Identifier: Nordic-5-Clause
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form, except as embedded into a Nordic
#    Semiconductor ASA integrated circuit in a product or a software update for
#    such product, must reproduce the above copyright notice, this list of
#    conditions and the following disclaimer in the documentation and/or other
#    materials provided with the distribution.
#
# 3. Neither the name of Nordic Semiconductor ASA nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
# 4. This software, with or without modification, must only be used with a
#    Nordic Semiconductor ASA integrated circuit.
#
# 5. Any software provided in binary form under this license must not be reverse
#    engineered, decompiled, modified and/or disassembled.
#
# THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
# OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
# OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 3.13.1)

set(REQ_C_COMPILER_VERSION 9.2.1)
set(REQ_C_COMPILER arm-none-eabi-gcc)
set(DESIRED_ZEPHYR_SDK_VERSION 0.13.1)
# Checks which compiler is used. GNU Arm Embedded Toolchain is strongly preferred.
# https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads
# E.g. for compiler version 10.2.1: the "10-2020-q4-major" release includes this. You have to check which
# GNU release contains the correct compiler.

if(DEFINED ENV{ZEPHYR_TOOLCHAIN_VARIANT})
    if ($ENV{ZEPHYR_TOOLCHAIN_VARIANT} STREQUAL "gnuarmemb")
        if(NOT DEFINED ENV{GNUARMEMB_TOOLCHAIN_PATH})
                message(FATAL_ERROR "The env variable GNUARMEMB_TOOLCHAIN_PATH is not set. It must point to the gnuarmemb folder")
        endif()
    else()
        message(FATAL_ERROR "The env variable ZEPHYR_TOOLCHAIN_VARIANT must be gnuarmemb. Value now: $ENV{ZEPHYR_TOOLCHAIN_VARIANT}" )
    endif()
else()
    message(FATAL_ERROR "The env variable ZEPHYR_TOOLCHAIN_VARIANT is not set. Should preferably be set to gnuarmemb")
endif()

if (NOT ${CMAKE_C_COMPILER} MATCHES ${REQ_C_COMPILER})
        message(FATAL_ERROR "The compiler is ${CMAKE_C_COMPILER} must be of type: ${REQ_C_COMPILER}")
endif()

if(NOT CMAKE_C_COMPILER_VERSION)
    message(FATAL_ERROR "CMAKE_C_COMPILER_VERSION was not automatically detected. Is GNU Arm Embedded Toolchain installed?")
elseif(NOT (CMAKE_C_COMPILER_VERSION VERSION_EQUAL ${REQ_C_COMPILER_VERSION}))
    message(FATAL_ERROR "The C compiler version is: ${CMAKE_C_COMPILER_VERSION}. It must be: ${REQ_C_COMPILER_VERSION}")
endif()

# The ZEPHYR_SDK_INSTALL_DIR may not be defined on all systems. If it exists, check version
if(DEFINED ENV{ZEPHYR_SDK_INSTALL_DIR})
    set(ZEPHYR_SDK_VERSION 0)
    file(READ $ENV{ZEPHYR_SDK_INSTALL_DIR}/sdk_version ZEPHYR_SDK_VERSION)
    if (NOT ZEPHYR_SDK_VERSION VERSION_EQUAL DESIRED_ZEPHYR_SDK_VERSION)
        message(WARNING "The Zephyr SDK version is: ${ZEPHYR_SDK_VERSION} - It should preferably be: ${DESIRED_ZEPHYR_SDK_VERSION}")
    else()
        message("Zephyr SDK version is ${ZEPHYR_SDK_VERSION}")
    endif()
endif()

