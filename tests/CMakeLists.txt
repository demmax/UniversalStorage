enable_testing()

find_package(GTest REQUIRED)
find_package(GMock REQUIRED)

set(MOCKS
        ${CMAKE_CURRENT_SOURCE_DIR}/mocks/MemoryStorage.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/mocks/MemoryStorage.h
        ${CMAKE_CURRENT_SOURCE_DIR}/mocks/MockPhysicalStorage.h)

add_subdirectory(unit)
add_subdirectory(integration)
add_subdirectory(performance)