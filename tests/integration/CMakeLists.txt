set(TestName StorageIntegrationTests)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/tests)

set(FIXTURE_SRC UniversalStorageFixture.hpp)
set(BASE_SRC BaseFunctionalityTest.cpp PerformanceTest.cpp ../mocks/MockBlockManager.h)
set(THREADING_SRC MultithreadingTest.cpp)
set(PERFORMANCE_SRC PerformanceTest.cpp)

set(BASE_BIN_NAME BaseFunctionalTest)
set(THREAD_BIN_NAME MultithreadingTest)
set(PERFORMANCE_BIN_NAME PerformanceTest)

set(LIBS ${PROJECT_NAME} ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(${BASE_BIN_NAME} ${FIXTURE_SRC} ${BASE_SRC} ${MOCKS})
target_link_libraries(${BASE_BIN_NAME} ${LIBS})


add_executable(${THREAD_BIN_NAME} ${FIXTURE_SRC} ${THREADING_SRC} ${MOCKS})
target_link_libraries(${THREAD_BIN_NAME} ${LIBS} ${Threads})


add_executable(${PERFORMANCE_BIN_NAME} ${FIXTURE_SRC} ${PERFORMANCE_SRC} ${MOCKS})
target_link_libraries(${PERFORMANCE_BIN_NAME} ${LIBS} ${Threads})