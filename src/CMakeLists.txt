set(SOURCES StorageAccessor.cpp DataSerializer.hpp)
set(HEADERS StorageAccessor.h IPhysicalStorage.h)


add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${PROJECT_NAME})