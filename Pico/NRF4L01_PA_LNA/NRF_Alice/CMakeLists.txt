# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(NRF_Alice C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

set(PICO_CXX_ENABLE_RTTI 1)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(NRF_Alice 
NRF_Alice.cpp 
#NRF24.cpp
)

pico_set_program_name(NRF_Alice "NRF_Alice")
pico_set_program_version(NRF_Alice "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(NRF_Alice 1)
pico_enable_stdio_usb(NRF_Alice 1)

# Add the standard library to the build
target_link_libraries(NRF_Alice
        pico_stdlib
        hardware_spi
        hardware_uart
        )

# Add the standard include files to the build
# target_include_directories(NRF_Alice PRIVATE
#         ${CMAKE_CURRENT_LIST_DIR}
# )

target_include_directories(NRF_Alice PUBLIC
        inc
        #${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(NRF_Alice 
        hardware_spi
        pico_stdlib
        )

pico_add_extra_outputs(NRF_Alice)

