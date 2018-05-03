//
// Created by maxon on 30.04.18.
//

#include "NaivePhysicalStorage.h"


bool NaivePhysicalStorage::isExist(const std::string &path) const
{

    return (m_dataMap.find(path) != m_dataMap.end());
}

void NaivePhysicalStorage::setValue(const std::string &path, const std::vector<uint8_t> &data)
{
    m_dataMap[path] = data;
}

std::vector<uint8_t> NaivePhysicalStorage::getValue(const std::string &path) const
{
    return m_dataMap.at(path);
}
