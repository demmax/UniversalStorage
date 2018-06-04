set(TestName PerformanceTest)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(SOURCES main.cpp)

set(LIBS ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})
add_executable(${TestName} ${SOURCES})
target_link_libraries(${TestName} ${LIBS})
