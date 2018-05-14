set(SOURCES StorageAccessor.hpp DataSerializer.hpp MountPointTree.cpp MountPointTree.h function_checking.hpp  PathView.cpp NaiveStorage.cpp CachingProxyStorage.cpp)
set(HEADERS IStorage.h exceptions.h PathView.h  NaiveStorage.h  CachingProxyStorage.h)


add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME})