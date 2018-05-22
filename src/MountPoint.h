//
// Created by maxon on 22.05.18.
//

#ifndef UNIVERSALSTORAGE_MOUNTPOINT_H
#define UNIVERSALSTORAGE_MOUNTPOINT_H

#include "IStorage.h"


namespace UniversalStorage {


struct MountPoint
{
    MountPoint(IStoragePtr s, std::string _ppath, std::string _mpath, uint32_t p);
    bool operator<(const MountPoint &o) const;
    std::string fullPath(const std::string &path) const;


    IStoragePtr storage;
    uint32_t priority;
    std::string physical_path;
    std::string mount_path;
};

}

#endif //UNIVERSALSTORAGE_MOUNTPOINT_H
