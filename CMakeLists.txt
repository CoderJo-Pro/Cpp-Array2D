cmake_minimum_required(VERSION 3.10)

project(
    Array2D
    VERSION 1.0.0
    DESCRIPTION "Grid support."
    LANGUAGES CXX)

file(GLOB SRC_FILES
    "${PROJECT_SOURCE_DIR}/include/arr2d/*.h")

add_library(Array2D ${SRC_FILES})
set_target_properties(
    Array2D
    PROPERTIES VERSION ${PROJECT_VERSION}
               SOVERSION ${PROJECT_VERSION_MAJOR})

set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED True)

target_include_directories(Array2D INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
