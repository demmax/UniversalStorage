//
// Created by maxon on 03.05.18.
//

#ifndef UNIVERSALSTORAGE_NAIVESTORAGE_H
#define UNIVERSALSTORAGE_NAIVESTORAGE_H

#include <map>
#include "IStorage.h"


#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/map.hpp>

using MappedFile = boost::interprocess::managed_mapped_file;
using MappedFileSegmentManager = MappedFile::segment_manager;
template <typename T> using MappedFileAllocator = boost::interprocess::allocator<T, MappedFileSegmentManager>;
using KeyType = boost::interprocess::basic_string<char, std::char_traits<char>, MappedFileAllocator<char>>;

struct storage_less
{
    bool operator() (const KeyType &v1, const KeyType &v2) const {
        return v1 < v2;
    }
};


class NaiveStorage : public IStorage
{
public:
    NaiveStorage(const std::string &file_path);

    void setValue(const std::string &path, const std::vector<uint8_t> &data) override;
    std::vector<uint8_t> getValue(const std::string &path) const override;
    bool isExist(const std::string &path) const override;
    ~NaiveStorage() = default;

protected:

    using ValueType = boost::interprocess::vector<uint8_t, MappedFileAllocator<uint8_t>>;
    using MapNodeType = std::pair<KeyType, ValueType>;
    using Map = boost::interprocess::map<KeyType, ValueType, std::less<>, MappedFileAllocator<std::pair<const KeyType, ValueType>>>;

    Map *p_data;
    MappedFile m_file;
};


#endif //UNIVERSALSTORAGE_NAIVESTORAGE_H
