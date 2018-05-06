//
// Created by maxon on 30.04.18.
//

#include "MemoryStorage.h"


bool MemoryStorage::isExist(const std::string &path) const
{

    return (m_dataMap.find(path) != m_dataMap.end());
}

void MemoryStorage::setValue(const std::string &path, const std::vector<uint8_t> &data)
{
    m_dataMap[path] = data;
}

std::vector<uint8_t> MemoryStorage::getValue(const std::string &path) const
{
    return m_dataMap.at(path);
}
