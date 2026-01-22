cmake_minimum_required(VERSION 3.10)

project(sbs
        VERSION 0.0.1
        DESCRIPTION "Simple Binary Serialization. A C++ Binary Serialization Library."
        HOMEPAGE_URL "https://github.com/orosmatthew/sbs"
        LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

option(SBS_BUILD_TESTS "SBS Build Tests" OFF)
option(SBS_WRITE_TEST_FILES "SBS Write Test Files" OFF)
option(SBS_BUILD_EXAMPLES "SBS Build Examples" OFF)

add_library(sbs INTERFACE)
target_include_directories(sbs INTERFACE include)

if (SBS_BUILD_TESTS)
    add_executable(sbs_tests tests/test_helper.cpp tests/main.cpp)
    target_link_libraries(sbs_tests PRIVATE sbs)
    if (SBS_WRITE_TEST_FILES)
        target_compile_definitions(sbs_tests PRIVATE SBS_WRITE_TEST_FILES)
    endif ()
endif ()

if (SBS_BUILD_EXAMPLES)
    add_executable(minimal_example examples/minimal_example.cpp)
    target_link_libraries(minimal_example PRIVATE sbs)
    add_executable(complete_example examples/complete_example.cpp)
    target_link_libraries(complete_example PRIVATE sbs)
endif ()