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

if("x-$ENV{ZEPHYR_BASE}" STREQUAL "x-")
    message(FATAL_ERROR
   "         ERROR            \n\
     YOU HAVE TO SET THE ZEPHYR_BASE ENV VARIABLE\n\
     This is usually set by running west update ")
endif("x-$ENV{ZEPHYR_BASE}" STREQUAL "x-")

# Set the nRF5340 Audio version. x.x.99 indicates master branch/cutting edge
set(NRF5340_AUDIO_RELEASE_VERSION 0.4.0)

# Set paths
set(NRF5340_AUDIO_REPO_ROOT ${CMAKE_CURRENT_LIST_DIR} CACHE PATH "nRF5340 Audio root directory")
set(ENV{NRF5340_AUDIO_REPO_ROOT} $NRF5340_AUDIO_REPO_ROOT)
message("NRF5340_AUDIO_REPO_ROOT is set to: ${NRF5340_AUDIO_REPO_ROOT}")

set(NRF5340_AUDIO_SRC ${NRF5340_AUDIO_REPO_ROOT}/src)
set(NRF5340_AUDIO_PROJECTS ${NRF5340_AUDIO_REPO_ROOT}/projects)
set(NRF5340_AUDIO_INC ${NRF5340_AUDIO_REPO_ROOT}/include)
set(EXT_ROOT ${NRF5340_AUDIO_REPO_ROOT}/ext)
set(NRF5340_AUDIO_VERIFICATION ${NRF5340_AUDIO_REPO_ROOT}/verification)
set(NRF5340_AUDIO_DEBUG_TOOLS_SRC ${NRF5340_AUDIO_REPO_ROOT}/tools/debug/src)
set(NRF5340_AUDIO_DEBUG_TOOLS_INC ${NRF5340_AUDIO_REPO_ROOT}/tools/debug/inc)
set(NRF5340_AUDIO_MODULES ${NRF5340_AUDIO_REPO_ROOT}/../modules)
set(LC3_ROOT ${NRF5340_AUDIO_REPO_ROOT}/../../../modules/lib/lc3)
set(CIRRUS_ROOT ${EXT_ROOT}/cirrus_logic)

# Set up variables used by the Zephyr build system
set(KCONFIG_ROOT ${NRF5340_AUDIO_REPO_ROOT}/Kconfig.nrf5340_audio)
message("KCONFIG_ROOT : ${KCONFIG_ROOT}")
set(BOARD_ROOT ${NRF5340_AUDIO_REPO_ROOT}/../..)
message("BOARD_ROOT : ${BOARD_ROOT}")

# Add core specific config
set(CONF_FILE "${CONF_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/prj.conf")

# Check if -DCMAKE_BUILD_TYPE=RELEASE, if so, add the prj_release.conf to the list of configs.
if(CMAKE_BUILD_TYPE MATCHES RELEASE)
    # Add generic release config
    set(CONF_FILE "${CONF_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/prj_release.conf")
elseif(CMAKE_BUILD_TYPE MATCHES DEBUG)
    message("
          ------------------------------------------------------------
          ---     DEBUG mode, enabling power hungry debug features ---
          ------------------------------------------------------------
          ")
    add_definitions(-DDEBUG)
    # Add core specific debug config
    set(CONF_FILE "${CONF_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/prj_debug.conf")
else()
    message(WARNING "CMAKE_BUILD_TYPE is neither RELEASE nor DEBUG. This may be OK for tests")
endif()
message("*.conf files used: ${CONF_FILE}")
