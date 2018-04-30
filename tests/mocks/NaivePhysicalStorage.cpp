//
// Created by maxon on 30.04.18.
//

#include "NaivePhysicalStorage.h"

void NaivePhysicalStorage::setValue(const std::string &path, const uint8_t *data, size_t size)
{
    m_dataMap[path] = std::vector<uint8_t>(data, data + size);
}

std::tuple<uint8_t*, size_t> NaivePhysicalStorage::getValue(const std::string &path) const
{
    auto &data = m_dataMap.at(path);
    return std::make_tuple<uint8_t*, size_t>(const_cast<uint8_t*>(data.data()), data.size());
}

bool NaivePhysicalStorage::isExist(const std::string &path) const
{

    return (m_dataMap.find(path) != m_dataMap.end());
}
