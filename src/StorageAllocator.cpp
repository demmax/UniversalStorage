//
// Created by maxon on 26.05.18.
//

#include "StorageAllocator.h"

#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/endian/conversion.hpp>

#include <boost/filesystem.hpp>


using namespace UniversalStorage;


UniversalStorage::StorageAllocator::StorageAllocator(std::string file_name) : m_fileName(file_name)
{
    if (!boost::filesystem::exists(file_name) || boost::filesystem::file_size(file_name) < HEADER_SIZE) {
        boost::filesystem::ofstream of(file_name);
        boost::filesystem::resize_file(file_name, HEADER_SIZE);
    }

    m_mappedFile = boost::iostreams::mapped_file(file_name, boost::iostreams::mapped_file::readwrite);
}

