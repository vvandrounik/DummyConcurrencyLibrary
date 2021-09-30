cmake_minimum_required(VERSION 3.10)

project(dcl)

add_library(dcl INTERFACE)

set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/inc")

target_include_directories (dcl INTERFACE ${INC_DIR})
