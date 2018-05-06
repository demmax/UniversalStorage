include_directories(${CMAKE_SOURCE_DIR}/src)

set(SOURCES StorageAccessorTests.cpp mocks/MemoryStorage.cpp mocks/MemoryStorage.h mocks/MockPhysicalStorage.h MountPointTreeTest.cpp PathViewTest.cpp NaiveStorageTests.cpp)
set(TestName StorageTests)
set(LIBS ${PROJECT_NAME} ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_executable(${TestName} ${SOURCES})

target_link_libraries(${TestName} ${LIBS})