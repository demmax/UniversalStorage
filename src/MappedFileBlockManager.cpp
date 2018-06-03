//
// Created by maxon on 26.05.18.
//

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include "MappedFileBlockManager.h"


uint8_t *UniversalStorage::MappedFileBlockManager::getRootBlock()
{
    return p_rootNodeBlock;
}


uint8_t *UniversalStorage::MappedFileBlockManager::getTreeNodeBlock(uint64_t offset)
{
    return p_rootNodeBlock + offset;
}


uint8_t *UniversalStorage::MappedFileBlockManager::getFreeTreeNodeBlock()
{
//    if (m_mappedFile.size() <= HEADER_SIZE + m_firstFreeBlockOffset * DATA_BLOCK_SIZE) {
//        m_mappedFile.resize(m_mappedFile.size() + SECTOR_DATA_SIZE);
//    }

    // Aquire contiguous blocks for tree node
//    int
}


void UniversalStorage::MappedFileBlockManager::freeTreeNodeBlock(uint8_t *address)
{

}


uint64_t UniversalStorage::MappedFileBlockManager::storeData(const std::vector<uint8_t> &data, uint64_t offset)
{
    return 0;
}


uint64_t UniversalStorage::MappedFileBlockManager::storeNewData(const std::vector<uint8_t> &data)
{
    return 0;
}


uint64_t UniversalStorage::MappedFileBlockManager::storePath(const std::string &path, uint64_t offset)
{
    return 0;
}


uint64_t UniversalStorage::MappedFileBlockManager::storeNewPath(const std::string &path)
{
    return 0;
}


void UniversalStorage::MappedFileBlockManager::freeBlock(uint64_t offset)
{

}


std::string UniversalStorage::MappedFileBlockManager::getPathFromBlock(uint64_t offset)
{
    return std::__cxx11::string();
}


std::vector<uint8_t> UniversalStorage::MappedFileBlockManager::getDataFromBlock(uint64_t offset)
{
    return std::vector<uint8_t>();
}


bool UniversalStorage::MappedFileBlockManager::isRootInitialized() const
{
    return false;
}


uint64_t UniversalStorage::MappedFileBlockManager::getOffset(uint8_t *address)
{
    return 0;
}


UniversalStorage::MappedFileBlockManager::MappedFileBlockManager(const std::string &file_name)
{
    static const size_t INITIAL_SIZE = HEADER_SIZE + BITMAP_SIZE + TREE_NODE_BLOCK_SIZE;
    if (!boost::filesystem::exists(file_name) || boost::filesystem::file_size(file_name) < INITIAL_SIZE) {
        boost::filesystem::ofstream of(file_name);
        boost::filesystem::resize_file(file_name, INITIAL_SIZE);
    }

    m_mappedFile = boost::iostreams::mapped_file(file_name, boost::iostreams::mapped_file::readwrite);
    p_rootNodeBlock = (uint8_t*)m_mappedFile.data() + HEADER_SIZE + BITMAP_SIZE;


//    m_mappedFile.resize(2048);
//    std::memcpy(m_mappedFile.data(), rootBlock.get(), 4096);
//    m_mappedFile.close();
}


int UniversalStorage::MappedFileBlockManager::getFreeBlockIndex() const
{
    return 0;
}
