set(TestName StorageUnitTests)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(SOURCES
        StorageAccessorTests.cpp
        MountPointTreeTest.cpp
        PathViewTest.cpp
        NaiveStorageTest.cpp
        LRUCachingProxyStorageTest.cpp)

set(LIBS ${PROJECT_NAME} ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_executable(${TestName} ${SOURCES} ${MOCKS})
target_link_libraries(${TestName} ${LIBS})