cmake_minimum_required(VERSION 3.10)

project(simple-binary-serialization)
set(CMAKE_CXX_STANDARD 20)

option(SBS_BUILD_TESTS "SBS Build Tests" OFF)
option(SBS_WRITE_TEST_FILES "SBS Write Test Files" OFF)

add_library(sbs INTERFACE)
target_include_directories(sbs INTERFACE include)

if (SBS_BUILD_TESTS)
    add_executable(sbs_tests tests/test_helper.cpp tests/main.cpp)
    target_link_libraries(sbs_tests PRIVATE sbs)
    if (SBS_WRITE_TEST_FILES)
        target_compile_definitions(sbs_tests PRIVATE SBS_WRITE_TEST_FILES)
    endif ()
endif ()