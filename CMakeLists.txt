# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(hello_world)

target_sources(app PRIVATE src/main.c)

# add wasm-micro-runtime dependencies.

target_sources(app PRIVATE ${app_sources})
include_directories(wamr/core/iwasm/include)

enable_language (ASM)

    set (WAMR_BUILD_AOT 1)
    set (WAMR_BUILD_LIBC_BUILTIN 1)
    set (WAMR_BUILD_TARGET "X86_32")
    set (WAMR_BUILD_PLATFORM "zephyr")
    set (WAMR_BUILD_APP_FRAMEWORK 1)
    set (WAMR_BUILD_APP_LIST WAMR_APP_BUILD_BASE)

set (WAMR_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/wamr)

include (${WAMR_ROOT_DIR}/build-scripts/runtime_lib.cmake)

target_sources(app PRIVATE
                ${WAMR_RUNTIME_LIB_SOURCE}
)
