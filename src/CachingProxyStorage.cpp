//
// Created by maxon on 06.05.18.
//

#include "CachingProxyStorage.h"

CachingProxyStorage::CachingProxyStorage(IStoragePtr storage) : p_storage(std::move(storage))
{
}

void CachingProxyStorage::setValue(const std::string &path, const std::vector<uint8_t> &data)
{

}

std::vector<uint8_t> CachingProxyStorage::getValue(const std::string &path) const
{

    return std::vector<uint8_t>();
}

bool CachingProxyStorage::isExist(const std::string &path) const
{

    return false;
}

