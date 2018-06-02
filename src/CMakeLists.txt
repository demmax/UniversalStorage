set(SOURCES StorageAccessor.hpp DataSerializer.hpp MountPointTree.cpp MountPointTree.h function_checking.hpp  PathView.cpp NaiveStorage.cpp LRUCachingProxyStorage.cpp)
set(HEADERS IStorage.h exceptions.h PathView.h  NaiveStorage.h LRUCachingProxyStorage.h MountPoint.cpp MountPoint.h PhysicalStorage.cpp PhysicalStorage.h StorageAllocator.cpp StorageAllocator.h BPTree.cpp BPTree.h IBlockManager.h MappedFileBlockManager.cpp MappedFileBlockManager.h utils.hpp)


add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME})