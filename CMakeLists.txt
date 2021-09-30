cmake_minimum_required(VERSION 3.10)

project(dummy_concurrency_library LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

message(STATUS "[Option] BUILD_TESTS: ${BUILD_TESTS}")

add_subdirectory(dcl)

if(BUILD_TESTS)
    include(GoogleTest)
    enable_testing()
    add_subdirectory(tests)
endif(BUILD_TESTS)
