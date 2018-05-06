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


class NaiveStorage : public IStorage
{
public:
    NaiveStorage(std::string file_path);

    void setValue(const std::string &path, const std::vector<uint8_t> &data) override;
    std::vector<uint8_t> getValue(const std::string &path) const override;
    bool isExist(const std::string &path) const override;
    ~NaiveStorage() override = default;

protected:
    using MappedFile = boost::interprocess::managed_mapped_file;
    template <typename T> using MappedFileAllocator = boost::interprocess::allocator<T, MappedFile::segment_manager>;
    using KeyType = boost::interprocess::basic_string<char, std::char_traits<char>, MappedFileAllocator<char>>;
    using ValueType = boost::interprocess::vector<uint8_t, MappedFileAllocator<uint8_t>>;
    using MapNodeType = std::pair<const KeyType, ValueType>;
    using Map = boost::interprocess::map<KeyType, ValueType, std::less<>, MappedFileAllocator<MapNodeType>>;

    void trySetValue(const std::string &path, const std::vector<uint8_t> &data);

    Map *p_data;
    std::unique_ptr<MappedFile> p_file;
    std::string m_fileName;

    static constexpr size_t INITIAL_SIZE = 4 * 1024;
};


#endif //UNIVERSALSTORAGE_NAIVESTORAGE_H
