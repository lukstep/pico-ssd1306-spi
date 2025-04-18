cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico_w)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(oled C CXX ASM)

pico_sdk_init()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -Wall -Wextra -Wpedantic -Wswitch -Wshadow")
message(STATUS "CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}")

add_executable(${CMAKE_PROJECT_NAME} main.cpp)

pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)
pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 1)
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})

target_link_libraries(${CMAKE_PROJECT_NAME} pico_stdlib hardware_spi)

# Dispaly size od the ELF sectionsś
set(CMAKE_SIZE arm-none-eabi-size)
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} --format=gnu --radix=10 $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
    COMMENT "Displaying section sizes of the ELF file..."
)

# Export path to project executable to file. File is used in vscode tasks (task.json and launch.json)
set(ELF_PATH "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_FILE}${CMAKE_PROJECT_NAME}.elf")
file(WRITE "${CMAKE_BINARY_DIR}/elf_path.txt" "${ELF_PATH}\n")