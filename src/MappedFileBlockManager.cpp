//
// Created by maxon on 26.05.18.
//

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <cmath>
#include <boost/endian/conversion.hpp>
#include "MappedFileBlockManager.h"

using namespace UniversalStorage;


uint8_t *UniversalStorage::MappedFileBlockManager::getRootBlock()
{
    return p_rootNodeBlock;
}


uint8_t *UniversalStorage::MappedFileBlockManager::getBlockPointer(uint64_t offset)
{
    return p_firstDataBlock + offset;
}


uint8_t *UniversalStorage::MappedFileBlockManager::getFreeTreeNodeBlock()
{
    auto offset = findContiguousFreeBlocks(DATA_BLOCKS_IN_TREE_NODE, m_firstFreeBlockOffset);
    for (auto i = 0u; i < DATA_BLOCKS_IN_TREE_NODE; i++)
        aquireBlock(offset + i * DATA_BLOCK_SIZE);

    return getBlockPointer(offset);
}


void UniversalStorage::MappedFileBlockManager::freeTreeNodeBlock(uint64_t offset)
{
    for (auto i = 0u; i < DATA_BLOCKS_IN_TREE_NODE; i++)
        internalFreeBlock(offset, false);
}


uint64_t UniversalStorage::MappedFileBlockManager::storeData(const std::vector<uint8_t> &data, uint64_t offset)
{
    return storeData(data.data(), data.size(), offset);
}


uint64_t UniversalStorage::MappedFileBlockManager::storeNewData(const std::vector<uint8_t> &data)
{
    auto offset = getOffset(getFreeBlock());
    return storeData(data, offset);
}


uint64_t UniversalStorage::MappedFileBlockManager::storePath(const std::string &path, uint64_t offset)
{
    auto endianed = boost::endian::native_to_little(path);
    return storeData(reinterpret_cast<const uint8_t*>(endianed.c_str()), path.size(), offset);
}


uint64_t UniversalStorage::MappedFileBlockManager::storeNewPath(const std::string &path)
{
    auto offset = getOffset(getFreeBlock());
    return storePath(path, offset);
}


void UniversalStorage::MappedFileBlockManager::freeBlock(uint64_t offset)
{
    internalFreeBlock(offset, true);
}


void MappedFileBlockManager::internalFreeBlock(uint64_t offset, bool continued)
{
    while (continued && offset) {
        uint64_t sector_number = getSectorNumber(offset);
        uint64_t block_number = getBlockNumber(offset) - 1;
        uint16_t byte_number = block_number / 8;
        uint8_t bit_number = block_number % 8;

        size_t fs = m_mappedFile.size();
        uint8_t *sector_ptr = getBlockPointer(sector_number * SECTOR_DATA_SIZE);
        sector_ptr[byte_number] &= ~(1 << bit_number);

        if (offset < m_firstFreeBlockOffset)
            m_firstFreeBlockOffset = offset;

        uint8_t *block_ptr = getBlockPointer(offset);
        auto next_block_ptr = reinterpret_cast<uint64_t *>(block_ptr + DATA_LENGTH_SIZE + PLAIN_BLOCK_DATA_SIZE);
        offset = boost::endian::little_to_native(*next_block_ptr);
    }
}


std::string UniversalStorage::MappedFileBlockManager::getPathFromBlock(uint64_t offset)
{
    std::vector<uint8_t> data = getDataFromBlock(offset);
    return std::string(data.begin(), data.end());
}


std::vector<uint8_t> UniversalStorage::MappedFileBlockManager::getDataFromBlock(uint64_t offset)
{
    std::vector<uint8_t> result;
    uint8_t *pointer = getBlockPointer(offset);
    auto data_size_ptr = reinterpret_cast<uint64_t*>(pointer);
    uint64_t data_size = boost::endian::little_to_native(*data_size_ptr);
    result.resize(data_size);

    auto left_data_size = data_size;
    uint64_t copied = 0;
    while (offset) {
        auto to_copy = std::min(PLAIN_BLOCK_DATA_SIZE, left_data_size);
        std::memcpy(&(result[copied]), pointer + DATA_LENGTH_SIZE, to_copy);
        auto offset_ptr = reinterpret_cast<uint64_t*>(pointer + PLAIN_BLOCK_DATA_SIZE + DATA_LENGTH_SIZE);
        offset = boost::endian::little_to_native(*offset_ptr);
        pointer = getBlockPointer(offset);
    }
    return result;
}


bool UniversalStorage::MappedFileBlockManager::isRootInitialized() const
{
    return m_rootInitialized;
}


uint64_t UniversalStorage::MappedFileBlockManager::getOffset(uint8_t *address) const
{
    return static_cast<uint64_t>(address - p_firstDataBlock);
}


UniversalStorage::MappedFileBlockManager::MappedFileBlockManager(const std::string &file_name)
{
    static const size_t INITIAL_SIZE = HEADER_SIZE + SECTOR_DATA_SIZE;
    if (!boost::filesystem::exists(file_name) || boost::filesystem::file_size(file_name) < INITIAL_SIZE) {
        boost::filesystem::ofstream of(file_name);
        boost::filesystem::resize_file(file_name, INITIAL_SIZE);
        m_rootInitialized = false;
    }

    m_mappedFile = boost::iostreams::mapped_file(file_name, boost::iostreams::mapped_file::readwrite);
    p_firstDataBlock = (uint8_t*)m_mappedFile.data() + HEADER_SIZE;
    p_rootNodeBlock = p_firstDataBlock + BITMAP_SIZE;

    auto version_ptr = reinterpret_cast<uint32_t*>(m_mappedFile.data());
    auto first_free_offset_ptr = reinterpret_cast<uint64_t *>((uint8_t *) m_mappedFile.data() + HEADER_SIZE - 8);
    if (m_rootInitialized) {
        m_firstFreeBlockOffset = boost::endian::little_to_native(*first_free_offset_ptr);
        m_version = boost::endian::little_to_native(*version_ptr);
    }
    else {
        m_version = 1;
        *version_ptr = m_version;
        m_firstFreeBlockOffset = getOffset(p_rootNodeBlock + TREE_NODE_BLOCK_SIZE);
        *first_free_offset_ptr = boost::endian::little_to_native(m_firstFreeBlockOffset);
        aquireBlock(getOffset(p_firstDataBlock + BITMAP_SIZE));
        aquireBlock(getOffset(p_firstDataBlock + BITMAP_SIZE + DATA_BLOCK_SIZE));
    }
}


uint64_t UniversalStorage::MappedFileBlockManager::findFreeBlockOffset(uint64_t min_offset)
{
    uint64_t sector = getSectorNumber(min_offset);
    uint8_t *sector_ptr = getBlockPointer(sector * SECTOR_DATA_SIZE);
    uint64_t block = getBlockNumber(min_offset) - 1;
    auto start_byte = block / 8;
    auto start_bit = block % 8;

    while (true) {
        auto sec_offset = getOffset(sector_ptr);
        while (m_mappedFile.size() < sec_offset + SECTOR_DATA_SIZE) {
            resizeFile(m_mappedFile.size() + SECTOR_DATA_SIZE);
            sector_ptr = getBlockPointer(sector * SECTOR_DATA_SIZE);
        }

        for (auto bi = start_byte; bi < DATA_BLOCK_SIZE; bi++) {
            uint8_t byte = sector_ptr[bi];
            for (auto bit = start_bit; bit < 8; bit++) {
                uint8_t mask = 1 << bit;
                if ( (byte & mask) != mask ) {
                    auto new_off = sector * SECTOR_DATA_SIZE + BITMAP_SIZE + (DATA_BLOCK_SIZE * (bi * 8 + bit));
                        return new_off;
                }
            }
        }

        start_byte = 0;
        start_bit = 0;
        sector_ptr += SECTOR_DATA_SIZE;
        ++sector;
    }
}


uint64_t UniversalStorage::MappedFileBlockManager::findContiguousFreeBlocks(uint64_t count, uint64_t pos)
{
    uint64_t last_free_off = findFreeBlockOffset(pos);
    uint64_t first_free_block = last_free_off;
    uint64_t found = 1;

    while (found < count) {
        uint64_t next_off = findFreeBlockOffset(last_free_off + DATA_BLOCK_SIZE);
        if (next_off == last_free_off + DATA_BLOCK_SIZE) {
            ++found;
        }
        else {
            found = 1;
            first_free_block = next_off;
        }
        last_free_off = next_off;
    }

    blocksCount += 2;
    uint64_t offset = first_free_block;
    for (int i = 0; i < count - 1; i++) {
        setNextBlock(offset, offset + DATA_BLOCK_SIZE);
        offset += DATA_BLOCK_SIZE;
    }
    setNextBlock(offset, 0);

    return first_free_block;
}


void UniversalStorage::MappedFileBlockManager::aquireBlock(uint64_t offset)
{
    uint64_t sector_number = getSectorNumber(offset);
    uint64_t block_number = getBlockNumber(offset) - 1;
    uint16_t byte_number = block_number / 8;
    uint8_t bit_number = block_number % 8;

    uint8_t *sector_ptr = getBlockPointer(sector_number * SECTOR_DATA_SIZE);
    sector_ptr[byte_number] |= (1 << bit_number);

    if (m_firstFreeBlockOffset == offset) {
        m_firstFreeBlockOffset = findFreeBlockOffset(m_firstFreeBlockOffset + DATA_BLOCK_SIZE);
    }
}


uint64_t MappedFileBlockManager::storeData(const uint8_t *data, uint64_t data_size, uint64_t offset)
{
    auto pointer = getBlockPointer(offset);
    auto blocks_count = std::ceil((double)data_size / PLAIN_BLOCK_DATA_SIZE);
    auto data_size_ptr = reinterpret_cast<uint64_t*>(pointer);
    *data_size_ptr = boost::endian::native_to_little(data_size);

    uint64_t left_data_size = data_size;
    auto to_copy = std::min(PLAIN_BLOCK_DATA_SIZE, left_data_size);
    for (auto i = 0u; i < blocks_count; i++) {
        std::memcpy(pointer + DATA_LENGTH_SIZE, data, to_copy);
        left_data_size -= to_copy;

        auto *next_block_ptr = reinterpret_cast<uint64_t*>(pointer + DATA_LENGTH_SIZE + PLAIN_BLOCK_DATA_SIZE);
        uint64_t next_block_off = boost::endian::little_to_native(*next_block_ptr);
        if (left_data_size) {
            to_copy = std::min(PLAIN_BLOCK_DATA_SIZE, left_data_size);
            if (next_block_off) {
                pointer = getBlockPointer(next_block_off);
                continue;
            }
            else {
                pointer = getFreeBlock();
                next_block_off = getOffset(pointer);
            }
        }
        else {
            next_block_off = 0;
        }
        *next_block_ptr = boost::endian::native_to_little(next_block_off);
    }

    return offset;
}


uint8_t *MappedFileBlockManager::getFreeBlock()
{
    ++blocksCount;
    auto free_offset = m_firstFreeBlockOffset;
    if (m_mappedFile.size() <= HEADER_SIZE + SECTOR_DATA_SIZE * (getSectorNumber(free_offset))) {
        resizeFile(HEADER_SIZE + SECTOR_DATA_SIZE * (getSectorNumber(free_offset)));
    }
    aquireBlock(free_offset);
    return p_firstDataBlock + free_offset;
}


MappedFileBlockManager::~MappedFileBlockManager()
{
    m_mappedFile.close();
}


uint64_t MappedFileBlockManager::getRootOffset() const
{
    return getOffset(p_firstDataBlock + BITMAP_SIZE);
}


uint64_t MappedFileBlockManager::getSectorNumber(uint64_t offset) const
{
    return offset / SECTOR_DATA_SIZE;
}


uint64_t MappedFileBlockManager::getBlockNumber(uint64_t offset) const
{
    return (offset % SECTOR_DATA_SIZE) / DATA_BLOCK_SIZE;
}


int MappedFileBlockManager::getFreeBlockIndex(uint8_t *bitmap, uint64_t start_index) const
{
    auto start_byte = start_index / 8;
    for (auto bi = start_byte; bi < DATA_BLOCK_SIZE; bi++) {
        uint8_t byte = bitmap[bi];
        for (auto bit = 0u; bit < 8; bit++) {
            uint8_t mask = 1 << bit;
            if ( (byte & mask) != mask )
                return (bi * 8) + bit;
        }
    }

    return (-1);
}


void MappedFileBlockManager::resizeFile(uint64_t new_size)
{
    m_mappedFile.resize(new_size);
    p_firstDataBlock = (uint8_t*)m_mappedFile.data() + HEADER_SIZE;
    p_rootNodeBlock = p_firstDataBlock + BITMAP_SIZE;
}


void MappedFileBlockManager::setNextBlock(uint64_t offset, uint64_t next_off)
{
    uint8_t *pointer = getBlockPointer(offset);
    auto offset_ptr = reinterpret_cast<uint64_t*>(pointer + PLAIN_BLOCK_DATA_SIZE + DATA_LENGTH_SIZE);
    *offset_ptr = boost::endian::native_to_little(next_off);
}
