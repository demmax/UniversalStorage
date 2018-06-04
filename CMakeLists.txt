cmake_minimum_required(VERSION 3.10)
project(UniversalStorage)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

#SET(CMAKE_C_COMPILER clang-6.0)
#SET(CMAKE_CXX_COMPILER clang++-6.0)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()

find_package(Threads REQUIRED)
find_package(Boost 1.60 REQUIRED COMPONENTS iostreams filesystem)


add_subdirectory(src)
add_subdirectory(tests)