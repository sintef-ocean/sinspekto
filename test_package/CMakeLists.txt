cmake_minimum_required(VERSION 3.13)
project(SinspektoTest CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
include("${CMAKE_BINARY_DIR}/conan_paths.cmake")

if(DEFINED ENV{ANDROID_ABI})
  message(STATUS "ANDROID_ABI defined, setting ANDROID ON")
  set(ANDROID ON CACHE INTERNAL "Enabled ANDROID")
endif()

if(ANDROID)
  set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ON)
endif()

find_package(sinspekto CONFIG REQUIRED)
add_executable(sinspektoTest sinspektotest.cpp)

target_link_libraries(sinspektoTest
  sinspekto::sinspekto
  Qt5::Qml
  Qt5::Quick
  Qt5::Widgets
  Qt5::Charts)

enable_testing()

add_test(NAME sinspektotest COMMAND sinspektoTest -platform offscreen)
