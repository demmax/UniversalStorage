//
// Created by maxon on 26.05.18.
//

#include "PhysicalStorage.h"


void UniversalStorage::PhysicalStorage::setValue(const std::string &path, const std::vector<uint8_t> &data)
{

}


std::vector<uint8_t> UniversalStorage::PhysicalStorage::getValue(const std::string &path) const
{
    return std::vector<uint8_t>();
}


void UniversalStorage::PhysicalStorage::removeValue(const std::string &path)
{

}


bool UniversalStorage::PhysicalStorage::isExist(const std::string &path) const
{
    return false;
}
