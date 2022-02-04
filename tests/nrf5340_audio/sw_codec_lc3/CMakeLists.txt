cmake_minimum_required(VERSION 3.10.0)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
include(../tests.cmake)
project(NONE)

add_compile_definitions(CONFIG_SW_CODEC_LC3_ENC_SAMPLE_RATE_48KHZ_SUPPORT=1)
add_compile_definitions(CONFIG_SW_CODEC_LC3_DEC_SAMPLE_RATE_48KHZ_SUPPORT=1)
add_compile_definitions(CONFIG_SW_CODEC_LC3_7_5_MS_FRAMESIZE=1)

# LC3
include(${LC3_ROOT}/CMakeLists.txt)

target_sources(app PRIVATE main.c)
zephyr_library_include_directories(app PRIVATE ${NRF5340_AUDIO_INC})
