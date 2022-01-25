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
configure_file("${NRF5340_AUDIO_SRC}/fw_info_app.c.in" "${CMAKE_BINARY_DIR}/fw_info_app.c" @ONLY)

# Target sources below are specific to the nRF5340 Audio DK HW

message("== Compiling for BOARD = ${BOARD} ==")

target_sources(app PRIVATE
    ${NRF5340_AUDIO_SRC}/main.c
    ${NRF5340_AUDIO_SRC}/error_handler.c
    ${CMAKE_BINARY_DIR}/fw_info_app.c
    ${NRF5340_AUDIO_SRC}/led.c
    ${NRF5340_AUDIO_SRC}/debug.c
    ${NRF5340_AUDIO_SRC}/pcm_stream_channel_modifier.c
    ${NRF5340_AUDIO_SRC}/sw_codec_select.c
    ${NRF5340_AUDIO_SRC}/audio_codec.c
    ${NRF5340_AUDIO_SRC}/data_fifo.c
    ${NRF5340_AUDIO_SRC}/drivers/ina231.c
    ${NRF5340_AUDIO_SRC}/drivers/max14690.c
    ${NRF5340_AUDIO_SRC}/ctrl_events.c
    ${NRF5340_AUDIO_SRC}/button_handler.c
    ${NRF5340_AUDIO_SRC}/power_module.c
    ${NRF5340_AUDIO_SRC}/streamctrl.c
    ${NRF5340_AUDIO_SRC}/audio_usb.c
    ${NRF5340_AUDIO_SRC}/audio_i2s.c
    ${NRF5340_AUDIO_SRC}/pmic.c
    ${NRF5340_AUDIO_SRC}/cs47l63_comm.c
    ${NRF5340_AUDIO_SRC}/hw_codec.c
    ${NRF5340_AUDIO_SRC}/sd_card.c
    ${NRF5340_AUDIO_SRC}/audio_datapath.c
    ${NRF5340_AUDIO_SRC}/audio_sync_timer.c
    ${NRF5340_AUDIO_SRC}/subsys/bluetooth/ble_core.c
    ${NRF5340_AUDIO_SRC}/subsys/bluetooth/ble_trans.c
    ${NRF5340_AUDIO_SRC}/subsys/bluetooth/ble_audio_services.c
    ${NRF5340_AUDIO_SRC}/subsys/bluetooth/ble_acl_common.c
    ${NRF5340_AUDIO_SRC}/subsys/bluetooth/ble_acl_gateway.c
    ${NRF5340_AUDIO_SRC}/subsys/bluetooth/ble_acl_headset.c
    ${NRF5340_AUDIO_SRC}/subsys/bluetooth/ble_hci_vsc.c
    ${NRF5340_AUDIO_SRC}/subsys/bluetooth/ble_audio_services.c
    ${NRF5340_AUDIO_SRC}/board_version.c
    ${NRF5340_AUDIO_SRC}/uicr.c
    ${NRF5340_AUDIO_SRC}/channel_assignment.c
    ${NRF5340_AUDIO_SRC}/tone.c
    ${NRF5340_AUDIO_SRC}/contin_array.c
    )

zephyr_library_include_directories(app PRIVATE ${NRF5340_AUDIO_SRC})
zephyr_library_include_directories(app PRIVATE ${NRF5340_AUDIO_SRC}/subsys/bluetooth)
zephyr_library_include_directories(app PRIVATE ${NRF5340_AUDIO_SRC}/drivers)
zephyr_library_include_directories(app PRIVATE ${NRF5340_AUDIO_REPO_ROOT}/../../boards/arm/nrf5340_audio_dk_nrf5340)

# Codecs
if (CONFIG_SW_CODEC_LC3 AND CONFIG_SW_CODEC_SBC)
    message(FATAL_ERROR "Only one codec can be selected in the current implementation, not able to compile for SBC and LC3 at same time")
endif()

# Bluedroid SBC
if (CONFIG_SW_CODEC_SBC)
    include(${EXT_ROOT}/bluedroid/CMakelists.txt)
endif()

# LC3
if (CONFIG_SW_CODEC_LC3)
    if (EXISTS ${LC3_ROOT}/CMakeLists.txt)
        include(${LC3_ROOT}/CMakeLists.txt)
    else()
        message(FATAL_ERROR "LC3 repository not found - Do 'west config manifest.group-filter +nrf5340_audio' and then 'west update'. If access is denied, contact local Nordic Semiconductor support or use SBC codec")
    endif()
endif()

# Cirrus HW codec resources
include(${CIRRUS_ROOT}/CMakeLists.txt)

# SD card
if (CONFIG_DISK_ACCESS_SDHC)
    target_sources(app PRIVATE ${NRF5340_AUDIO_SRC}/sd_card.c)
endif()
