//
// Created by maxon on 26.05.18.
//

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include "MappedFileBlockManager.h"


uint8_t *UniversalStorage::MappedFileBlockManager::getRootBlock()
{
    return nullptr;
}


uint8_t *UniversalStorage::MappedFileBlockManager::getTreeNodeBlock(uint64_t offset)
{
    return nullptr;
}


uint8_t *UniversalStorage::MappedFileBlockManager::getFreeTreeNodeBlock()
{
    return nullptr;
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
    if (!boost::filesystem::exists(file_name) || boost::filesystem::file_size(file_name) < 4096) {
        boost::filesystem::ofstream of(file_name);
        boost::filesystem::resize_file(file_name, 4096);
    }

    m_mappedFile = boost::iostreams::mapped_file(file_name, boost::iostreams::mapped_file::readwrite);
    m_mappedFile.resize(2048);
//    std::memcpy(m_mappedFile.data(), rootBlock.get(), 4096);
    m_mappedFile.close();
}
