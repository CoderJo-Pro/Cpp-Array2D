cmake_minimum_required(VERSION 3.10)

project(GridTest LANGUAGES CXX)
include_directories(../include)

set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED True)

file(GLOB SRC_FILES
    "${PROJECT_SOURCE_DIR}/*_test.cpp")

include(FetchContent)
FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.0
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(catch2)

# These tests can use the Catch2-provided main
add_executable(tests ${SRC_FILES})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/catch2/extras")

enable_testing()
include(CTest)
include(Catch)
catch_discover_tests(tests)
