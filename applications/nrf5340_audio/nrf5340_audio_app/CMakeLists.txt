#
# Copyright (c) 2022 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.10.0)

include(../projects.cmake)

# Create flag which defines whether to compile for the gateway/dongle or headset
add_compile_definitions(NRF5340_AUDIO_DEV_HEADSET=1)
add_compile_definitions(NRF5340_AUDIO_DEV_GATEWAY=2)

option(DEV_HEADSET "Compile for headset hardware" OFF)
option(DEV_GATEWAY "Compile for gateway (PC dongle) hardware" OFF)

if (DEV_HEADSET AND DEV_GATEWAY)
    message(FATAL_ERROR "The selected device cannot be both HEADSET and GATEWAY")
elseif (NOT DEV_HEADSET AND NOT DEV_GATEWAY)
    message(FATAL_ERROR "The selected device must be either HEADSET or GATEWAY")
elseif (DEV_HEADSET)
    add_compile_definitions(NRF5340_AUDIO_DEV=NRF5340_AUDIO_DEV_HEADSET)
    set(CONF_FILE "${CONF_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/prj_headset.conf")
elseif (DEV_GATEWAY)
    add_compile_definitions(NRF5340_AUDIO_DEV=NRF5340_AUDIO_DEV_GATEWAY)
    set(CONF_FILE "${CONF_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/prj_gateway.conf")
else()
    message(FATAL_ERROR "Neither HEADSET or GATEWAY has been set")
endif()

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(NRF5340_AUDIO
        VERSION ${NRF5340_AUDIO_RELEASE_VERSION})

include(${NRF5340_AUDIO_REPO_ROOT}/verification/verify.cmake)

# Get the git commit
execute_process(COMMAND
  "${GIT_EXECUTABLE}" describe --match=DoNoTmAtCh --always --dirty
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE NRF5340_AUDIO_CORE_APP_GIT_SHA1
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

# Get the last commit date
execute_process(COMMAND
  "${GIT_EXECUTABLE}" log -1 --format=%ad --date=local
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE NRF5340_AUDIO_CORE_APP_GIT_DATE
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

string(TIMESTAMP NRF5340_AUDIO_CORE_APP_COMP_DATE "%a %b %d %H:%M:%S %Y")

# Generate fw_info_app.c
configure_file("${NRF5340_AUDIO_SRC}/utils/fw_info_app.c.in"
               "${CMAKE_BINARY_DIR}/fw_info_app.c"
               @ONLY)

# Target sources below are specific to the nRF5340 Audio DK HW

message("== Compiling for BOARD = ${BOARD} ==")

target_sources(app PRIVATE
    src/main.c
    ${CMAKE_BINARY_DIR}/fw_info_app.c
    )

# Include application events and configuration headers
zephyr_library_include_directories(
    src/audio
    src/bluetooth
    src/drivers
    src/events
    src/modules
    src/utils
    src/utils/macros
    )

zephyr_library_include_directories(app PRIVATE
        ${NRF5340_AUDIO_REPO_ROOT}/../../boards/arm/nrf5340_audio_dk_nrf5340)

# Application sources
add_subdirectory(src/audio)
add_subdirectory(src/bluetooth)
add_subdirectory(src/drivers)
add_subdirectory(src/events)
add_subdirectory(src/modules)
add_subdirectory(src/utils)

## Bluedroid SBC
if (CONFIG_SW_CODEC_SBC)
    include(${EXT_ROOT}/bluedroid/CMakelists.txt)
endif()

## LC3
if (CONFIG_SW_CODEC_LC3)
    if (EXISTS ${LC3_ROOT}/CMakeLists.txt)
        include(${LC3_ROOT}/CMakeLists.txt)
    else()
        message(FATAL_ERROR "LC3 repository not found\n"
                "Do 'west config manifest.group-filter +nrf5340_audio' "
                "and then 'west update'.\n"
                "If access is denied, contact local Nordic Semiconductor "
                "support or use SBC codec")
    endif()
endif()

## Cirrus HW codec resources
include(${CIRRUS_ROOT}/CMakeLists.txt)
