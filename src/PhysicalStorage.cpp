//
// Created by maxon on 26.05.18.
//

#include <boost/endian/conversion.hpp>
#include "PhysicalStorage.h"
#include "exceptions.h"


void UniversalStorage::PhysicalStorage::setValue(const std::string &path, const std::vector<uint8_t> &data)
{
    auto path_hash = hash(path);

    // Check if path already exists. In such case, just update data offset.
//    DataRecord value = m_btree.getValue(path_hash, path);
//    storeData(data, value.data);
//
//    // New path.
//    auto block = storeDataInNewBlock(reinterpret_cast<const uint8_t*>(boost::endian::native_to_little(path).c_str()), path.size());
//    if (data.size() > 8) {
//        auto data_bytes = storeDataInNewBlock(data.data(), data.size());
//        m_btree.setValue(path_hash, data_bytes, block, false);
//    }
//    else {
//        auto data_bytes = packVector(data);
//        m_btree.setValue(path_hash, data_bytes, block, true);
//    }
//
//    m_btree.store();
//    m_blockManager.sync();
}


std::vector<uint8_t> UniversalStorage::PhysicalStorage::getValue(const std::string &path) const
{
//    auto path_hash = hash(path);
//    DataRecord value = m_btree.getValue(path_hash, path);
//    if (value.is_data) {
//        return unpackValue(value.data);
//    }
//    else {
//        return getDataFromOffset(value.data);
//    }
}


void UniversalStorage::PhysicalStorage::removeValue(const std::string &path)
{
    auto path_hash = hash(path);

//    std::vector<DataRecord> values = m_btree.getValue(path_hash);
//    for (const auto &data_item : values) {
//        const std::string &data_path = getStringFromPathOffset(data_item.path_offset);
//        if (path == data_path) {
//            m_btree.removeKey(path_hash, data_item.path_offset);
//            return;
//        }
//    }

    throw NoSuchPathException(path.c_str());
}


bool UniversalStorage::PhysicalStorage::isExist(const std::string &path) const
{
    return false;
}


uint8_t *UniversalStorage::PhysicalStorage::storeData(const std::vector<uint8_t> &data, uint64_t offset)
{
    return nullptr;
}


uint64_t UniversalStorage::PhysicalStorage::hash(const std::string &str) const
{
    return 0;
}


uint64_t UniversalStorage::PhysicalStorage::storeDataInNewBlock(const uint8_t *data, size_t data_size)
{
    return 0;
}


std::string UniversalStorage::PhysicalStorage::getStringFromPathOffset(uint64_t) const
{
    return std::__cxx11::string();
}


uint64_t UniversalStorage::PhysicalStorage::packVector(const std::vector<uint8_t> &vec) const
{
    return 0;
}


std::vector<uint8_t> UniversalStorage::PhysicalStorage::unpackValue(uint64_t data) const
{
    return std::vector<uint8_t>();
}


std::vector<uint8_t> UniversalStorage::PhysicalStorage::getDataFromOffset(uint64_t offset) const
{
    return std::vector<uint8_t>();
}
