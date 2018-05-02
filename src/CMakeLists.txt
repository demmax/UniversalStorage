set(SOURCES StorageAccessor.cpp DataSerializer.hpp MountPointTree.cpp MountPointTree.h)
set(HEADERS StorageAccessor.h IStorage.h exceptions.h PathView.cpp PathView.h)


add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME})