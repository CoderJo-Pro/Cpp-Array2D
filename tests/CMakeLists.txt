cmake_minimum_required(VERSION 3.10)

project(GridTest LANGUAGES CXX)
include_directories(../include)

set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED True)

file(GLOB SRC_FILES
    "${PROJECT_SOURCE_DIR}/*_test.cpp")


add_subdirectory(lib/catch2)

# These tests can use the Catch2-provided main
add_executable(tests ${SRC_FILES})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
include(CTest)
add_test(NAME GridTest COMMAND tests)
