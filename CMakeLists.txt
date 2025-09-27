cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME C_Board_Standard_Robot)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE

    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    User/Algorithm/kalman_filter.c
    User/Algorithm/PID.cpp
    User/Algorithm/QuaternionEKF.c

    User/Application/ins_task.c
    User/Application/main_task.cpp

    User/Bsp/bsp_can.cpp
    User/Bsp/bsp_dwt.c
    User/Bsp/bsp_PWM.c
    User/Bsp/bsp_usart.c
    User/Bsp/bsp_crc.c


    User/Devices/BMI088/BMI088driver.c
    User/Devices/BMI088/BMI088Middleware.c
    User/Devices/BMI088/transfer_function.c
    User/Devices/MotorSynthesis/DaMiao.cpp
    User/Devices/MotorSynthesis/Motor.cpp
    User/Devices/Referee/Referee.c

    User/Include/controller.c
    User/Include/user_lib.c

    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    User/Algorithm
    User/Application
    User/Bsp
    User/Devices/BMI088
    User/Devices/MotorSynthesis
    User/Devices/Referee
    User/Include

    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE

    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
