//
// Created by maxon on 26.05.18.
//

#ifndef UNIVERSALSTORAGE_STORAGEALLOCATOR_H
#define UNIVERSALSTORAGE_STORAGEALLOCATOR_H

#include <memory>
#include <vector>
#include <boost/iostreams/device/mapped_file.hpp>

namespace UniversalStorage {


struct header
{
    uint64_t version;
    uint64_t file_size;
    uint64_t reserved;
};

struct sector
{
    uint64_t blocks_bitmap;
    uint8_t  free_block_offset;
};

struct block
{
    uint8_t data[1016];
    uint64_t next_block;
};


class StorageAllocator : public std::allocator<std::vector<uint8_t>>
{
public:
    typedef std::vector<uint8_t> data_type;
    typedef size_t  size_type;
    typedef data_type* pointer;

    StorageAllocator(std::string file_name);



protected:
    std::string m_fileName;
    header m_header;
    boost::iostreams::mapped_file m_mappedFile;

    static constexpr size_t HEADER_SIZE = 24; // NOTE: can't use sizeof because of portability: struct aligment not standardized
};


}

#endif //UNIVERSALSTORAGE_STORAGEALLOCATOR_H
