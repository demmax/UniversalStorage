//
// Created by maxon on 29.04.18.
//

#include "StorageAccessor.h"
#include <boost/endian/conversion.hpp>


void StorageAccessor::mountPhysicalVolume(std::shared_ptr<IStorage> storage, const std::string &mount_point,
                                         size_t priority, const std::string &path)
{
    m_storageTree.addMountPoint(mount_point, storage, path, priority);
}

std::string StorageAccessor::getFullPhysicalPath(const std::string &request_path, const std::string &mount_path, const std::string &phys_path) const
{
    std::string result = request_path;
    return phys_path + (result.erase(0, mount_path.length()));
}

IStoragePtr StorageAccessor::getForegroundPhysicalStorage(const std::string &path) const
{

    return IStoragePtr();
}

