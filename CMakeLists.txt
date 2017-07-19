cmake_minimum_required(VERSION 2.6)
project(RTL)

set(RTL_OUTPUT_BIN_DIR ${RTL_BINARY_DIR}/bin)

make_directory(${RTL_OUTPUT_BIN_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${RTL_OUTPUT_BIN_DIR}")

add_subdirectory(examples)
