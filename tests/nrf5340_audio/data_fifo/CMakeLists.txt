cmake_minimum_required(VERSION 3.10.0)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
include(../tests.cmake)
project(NONE)

target_sources(app PRIVATE
	${NRF5340_AUDIO_SRC}/data_fifo.c
	main.c)
zephyr_library_include_directories(app PRIVATE ${NRF5340_AUDIO_INC})
