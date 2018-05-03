//
// Created by maxon on 03.05.18.
//

#include "NaiveStorage.h"
#include <iostream>


NaiveStorage::NaiveStorage(const std::string &file_path)
        : m_file(boost::interprocess::open_or_create, file_path.c_str(), 50ul << 30)
{
    p_data = m_file.find_or_construct<Map>("DATA")(std::less<>(), Map::allocator_type(m_file.get_segment_manager()));
    std::cout << "Free memory: " << m_file.get_free_memory() << std::endl;

//    KeyType k1(m_file.get_segment_manager());
//    k1.assign("alloc");
//    KeyType k2(m_file.get_segment_manager());
//    k2.assign("alloc2");
//
//    std::cout << std::less<>()(k1, k2);
}


void NaiveStorage::setValue(const std::string &path, const std::vector<uint8_t> &data)
{
    auto allocator = m_file.get_segment_manager();
    KeyType key(allocator);
    key.assign(path.begin(), path.end());
    ValueType val(allocator);
    val.assign(data.begin(), data.end());
    p_data->emplace(key, val);
}

std::vector<uint8_t> NaiveStorage::getValue(const std::string &path) const
{
    KeyType key(m_file.get_segment_manager());
    key.assign(path.begin(), path.end());
    ValueType val = p_data->at(key);
    return std::vector<uint8_t>(val.begin(), val.end());
}

bool NaiveStorage::isExist(const std::string &path) const
{

    return false;
}

