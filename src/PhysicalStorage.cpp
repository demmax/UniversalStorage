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
    std::vector<RealData> values = m_btree.getValues(path_hash);
    for (const auto &data_item : values) {
        const std::string &data_path = getStringFromPathOffset(data_item.path_offset);
        if (path == data_path) {
            storeData(data, data_item.data);
            return;
        }
    }

    // New path.
    auto block = storeDataInNewBlock(reinterpret_cast<const uint8_t*>(boost::endian::native_to_little(path).c_str()), path.size());
    if (data.size() > 8) {
        auto data_bytes = storeDataInNewBlock(data.data(), data.size());
        m_btree.addKey(path_hash, data_bytes, block, false);
    }
    else {
        auto data_bytes = packVector(data);
        m_btree.addKey(path_hash, data_bytes, block, true);
    }

    m_btree.store();
//    m_blockManager.sync();
}


std::vector<uint8_t> UniversalStorage::PhysicalStorage::getValue(const std::string &path) const
{
    auto path_hash = hash(path);
    std::vector<RealData> values = m_btree.getValues(path_hash);
    for (const auto &data_item : values) {
        const std::string &data_path = getStringFromPathOffset(data_item.path_offset);
        if (path == data_path) {
            if (data_item.is_data) {
                return unpackValue(data_item.data);
            }
            else {
                return getDataFromOffset(data_item.data);
            }
        }
    }

    throw NoSuchPathException(path.c_str());
}


void UniversalStorage::PhysicalStorage::removeValue(const std::string &path)
{
    auto path_hash = hash(path);

    std::vector<RealData> values = m_btree.getValues(path_hash);
    for (const auto &data_item : values) {
        const std::string &data_path = getStringFromPathOffset(data_item.path_offset);
        if (path == data_path) {
            m_btree.removeKey(path_hash, data_item.path_offset);
            return;
        }
    }

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
