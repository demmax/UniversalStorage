//
// Created by maxon on 22.05.18.
//

#include <boost/container/string.hpp>
#include "MountPoint.h"
#include "IStorage.h"

using namespace UniversalStorage;


MountPoint::MountPoint(IStoragePtr s, std::string _ppath, std::string _mpath, uint32_t p)
        : storage(std::move(s)), physical_path(std::move(_ppath)), mount_path(std::move(_mpath)), priority(p)
{

}

std::string MountPoint::fullPath(const std::string &path) const
{
    std::string result = path;
    return physical_path + (result.erase(0, mount_path.length()));
}


bool MountPoint::operator<(const MountPoint &o) const
{
    return priority > o.priority;
}

