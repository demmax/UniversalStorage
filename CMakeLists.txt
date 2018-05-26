cmake_minimum_required(VERSION 3.10)
project(UniversalStorage)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(Threads REQUIRED)
find_package(Boost 1.60 REQUIRED COMPONENTS iostreams filesystem)

message("!" ${Boost_LIBRARIES})


add_subdirectory(src)
add_subdirectory(tests)