//
// Created by maxon on 03.05.18.
//

#include "NaiveStorage.h"
#include <iostream>
#include <boost/interprocess/exceptions.hpp>
#include <memory>


NaiveStorage::NaiveStorage(std::string file_path)
        : m_fileName(std::move(file_path))
{
    p_file = std::make_unique<MappedFile>(boost::interprocess::open_or_create, m_fileName.c_str(), INITIAL_SIZE);
    p_data = p_file->find_or_construct<Map>("DATA")(std::less<>(), Map::allocator_type(p_file->get_segment_manager()));
}


void NaiveStorage::setValue(const std::string &path, const std::vector<uint8_t> &data)
{
    try {
        trySetValue(path, data);
    }
    catch (const boost::interprocess::bad_alloc &ex) {
        size_t file_size = p_file->get_size();
        p_file.reset();
        boost::interprocess::managed_mapped_file::grow(m_fileName.c_str(), std::max(path.size() + data.size(), file_size));
        p_file = std::make_unique<MappedFile>(boost::interprocess::open_or_create, m_fileName.c_str(), INITIAL_SIZE);
        p_data = p_file->find_or_construct<Map>("DATA")(std::less<>(), Map::allocator_type(p_file->get_segment_manager()));
        trySetValue(path, data);
    }
}

std::vector<uint8_t> NaiveStorage::getValue(const std::string &path) const
{
    KeyType key(p_file->get_segment_manager());
    key.assign(path.begin(), path.end());
    ValueType val = p_data->at(key);
    return std::vector<uint8_t>(val.begin(), val.end());
}

bool NaiveStorage::isExist(const std::string &path) const
{

    return false;
}

void NaiveStorage::trySetValue(const std::string &path, const std::vector<uint8_t> &data)
{
    auto allocator = p_file->get_segment_manager();
    KeyType key(allocator);
    key.assign(path.begin(), path.end());
    ValueType val(allocator);
    val.assign(data.begin(), data.end());
    p_data->emplace(key, val);
}

