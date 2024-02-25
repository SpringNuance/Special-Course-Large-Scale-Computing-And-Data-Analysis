## Project settings
cmake_minimum_required(VERSION 3.21)

project(compression-test C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

## Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release") # Default
endif()
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

## Optimization flags
set(CFLAGS "-fopenmp -march=native -flto")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFLAGS}")

## Include directories
include_directories(.)

## External libraries
add_subdirectory(external/zfp)
#add_subdirectory(external/nvcomp)

## Warning flags
set(WFLAGS "-pedantic -Wall -Wextra -Wdouble-promotion\
            -Wfloat-conversion -Wno-error=unused-function\
		        -Wno-error=unused-parameter -Wno-error=unused-variable\
		        -Wshadow")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WFLAGS}")

add_subdirectory(src)
