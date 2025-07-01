cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico_w)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

set(LIB_NAME "ssd1306")

project(${LIB_NAME} C CXX ASM)

pico_sdk_init()

add_library(${LIB_NAME} STATIC src/ssd1306.cpp src/ssd1306_hw_driver.cpp)

target_include_directories(${LIB_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${LIB_NAME} PUBLIC pico_stdlib hardware_spi)

option(BUILD_EXAMPLES "Build example programs" OFF)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
