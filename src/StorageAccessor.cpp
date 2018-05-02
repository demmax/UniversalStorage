//
// Created by maxon on 29.04.18.
//

#include "StorageAccessor.h"
#include <boost/endian/conversion.hpp>


int StorageAccessor::mountPhysicalVolume(std::shared_ptr<IStorage> storage, const std::string &mount_point,
                                         int priority, const std::string &path)
{
    m_storage = storage;
    return 0;
}

std::string StorageAccessor::getFullPhysicalPath(IStoragePtr storage, const std::string &rel_path) const
{
    return rel_path;
}

IStoragePtr StorageAccessor::getForegroundPhysicalStorage(const std::string &path) const
{

    return IStoragePtr();
}

