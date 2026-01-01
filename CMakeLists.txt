cmake_minimum_required(VERSION 3.10)

project(simple-binary-serializer)
set(CMAKE_CXX_STANDARD 20)

option(SBS_BUILD_TESTS "SBS Build Tests" OFF)

add_library(sbs INTERFACE)
target_include_directories(sbs INTERFACE include)

if (SBS_BUILD_TESTS)
    add_executable(sbs_tests tests/main.cpp)
    target_link_libraries(sbs_tests PRIVATE sbs)
endif ()