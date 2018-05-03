//
// Created by maxon on 29.04.18.
//

#ifndef UNIVERSALSTORAGE_STORAGEACCESSOR_H
#define UNIVERSALSTORAGE_STORAGEACCESSOR_H


#include <vector>
#include <boost/endian/conversion.hpp>
#include "IStorage.h"


class StorageAccessor
{
public:
    StorageAccessor() = default;
    StorageAccessor(StorageAccessor &&o) = default;

    template <typename T> T getValue(const std::string &path);
    template <typename T> void setValue(const std::string &path, const T &val);

    void removeKey(const std::string &path);

    int mountPhysicalVolume(IStoragePtr storage,
                            const std::string &mount_point, int priority, const std::string &path = "/");


protected:
    IStoragePtr getForegroundPhysicalStorage(const std::string &path) const;
    std::string getFullPhysicalPath(IStoragePtr storage, const std::string &rel_path) const;

    IStoragePtr m_storage;
};


template<typename T>
void StorageAccessor::setValue(const std::string &path, const T &val)
{
    auto endianed_val = boost::endian::native_to_little(val);
    uint8_t *data = reinterpret_cast<uint8_t*>(&endianed_val);
    std::vector<uint8_t> data_vec;
    data_vec.assign(data, data + sizeof(endianed_val));
    m_storage->setValue(getFullPhysicalPath(m_storage, path), data_vec);
}

template <>
void StorageAccessor::setValue<std::vector<uint8_t>>(const std::string &path, const std::vector<uint8_t> &vec)
{
    m_storage->setValue(getFullPhysicalPath(m_storage, path), vec);
}


template <>
void StorageAccessor::setValue<std::string>(const std::string &path, const std::string &str)
{
    auto endianed_val = boost::endian::native_to_little(str);
    const uint8_t *mem = reinterpret_cast<const uint8_t*>(endianed_val.c_str());
    std::vector<uint8_t> data_vec;
    data_vec.assign(mem, mem + endianed_val.size());
    data_vec.push_back('\0');
    m_storage->setValue(getFullPhysicalPath(m_storage, path), data_vec); // Don't forget NULL-terminator
}


template<typename T>
T StorageAccessor::getValue(const std::string &path)
{
    auto data_vector = m_storage->getValue(getFullPhysicalPath(m_storage, path));
    auto endianed_val = reinterpret_cast<const T*>(data_vector.data());
    return boost::endian::little_to_native(*endianed_val);
}


template <>
std::string StorageAccessor::getValue<std::string>(const std::string &path)
{
    auto blob = m_storage->getValue(getFullPhysicalPath(m_storage, path));
    if (blob.back() != '\0')
        throw std::runtime_error("No NULL-terminator in data!"); // avoid UB

    auto endianed_val = reinterpret_cast<const char*>(blob.data());
    return std::string(boost::endian::little_to_native(endianed_val));
}

template <>
std::vector<uint8_t> StorageAccessor::getValue<std::vector<uint8_t>>(const std::string &path)
{
    return m_storage->getValue(getFullPhysicalPath(m_storage, path));
}


#endif // UNIVERSALSTORAGE_STORAGEACCESSOR_H
