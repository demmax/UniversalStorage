cmake_minimum_required(VERSION 3.10)
project(UniversalStorage)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)
find_package(GMock REQUIRED)
find_package(Boost 1.60 REQUIRED)

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)