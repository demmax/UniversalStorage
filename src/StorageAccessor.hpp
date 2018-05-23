//
// Created by maxon on 29.04.18.
//

#ifndef UNIVERSALSTORAGE_STORAGEACCESSOR_H
#define UNIVERSALSTORAGE_STORAGEACCESSOR_H


#include <vector>
#include <boost/endian/conversion.hpp>
#include <queue>
#include <thread>
#include <future>
#include "IStorage.h"
#include "MountPointTree.h"
#include "exceptions.h"
#include "function_checking.hpp"

namespace UniversalStorage {

class StorageAccessor
{
public:
    StorageAccessor();
    ~StorageAccessor();
    StorageAccessor(StorageAccessor &&o) = default;

    template<typename T> T getValue(const std::string &path);
    template<typename T> void setValue(const std::string &path, const T &val, std::chrono::milliseconds timeout = std::chrono::milliseconds::zero());
    void removeValue(const std::string &path);
    void mountPhysicalVolume(IStoragePtr storage,
                             const std::string &mount_point, size_t priority, const std::string &path = "/");
    void umountVolume(const std::string &path);
protected:
    template<typename T> void setValueImpl(const std::string &path, const T &val, std::chrono::milliseconds timeout);
    void timedKeyDestroyerThread();

    struct KeyTimeout
    {
        std::string path;
        std::chrono::steady_clock::time_point timeout;
        KeyTimeout(std::string s, std::chrono::steady_clock::time_point t) : path(std::move(s)), timeout(std::move(t)) {}
        bool operator>(const KeyTimeout &o) const { return timeout > o.timeout; }
    };

    MountPointTree m_storageTree;
    std::priority_queue<KeyTimeout, std::vector<KeyTimeout>, std::greater<>> m_timedKeysQueue;
    std::mutex m_timedQueueMtx;
    std::condition_variable m_cv;
    bool m_exit{false};
    std::thread m_destroyerThread;
};


using StorageAccessorPtr = std::shared_ptr<StorageAccessor>;

// Implementation


template<typename T>
void StorageAccessor::setValue(const std::string &path, const T &val, std::chrono::milliseconds timeout)
{
    if constexpr (std::is_array_v<T> && std::is_same_v<std::decay_t<T>, char*>) {
        // Implicit char array casting to avoid 'function returning an array' error from sfinae templates
        setValueImpl(path, static_cast<const char*>(val), timeout);
    }
    else if constexpr (TypeTraits::can_serialize<T>::value && TypeTraits::can_deserialize<T>::value) {
        if constexpr (TypeTraits::has_serialize_method<T>::value)
            setValueImpl(path, val.serialize(), timeout);
        else
            setValueImpl(path, TypeTraits::serialize<T>(val), timeout);
    } else {
        static_assert(!std::is_pointer_v<T> || std::is_same_v<T, const char *> || std::is_same_v<T, char *>,
                      "Non-pointer type or NULL-terminated string expected");
        static_assert(std::is_trivially_copy_constructible_v<T> || std::is_same_v<T, std::string>,
                      "Only trivially-copy-constructible types may be stored (or implement serialize/deserialize methods)");
        setValueImpl(path, val, timeout);
    }

    if (timeout != std::chrono::milliseconds::zero()) {
        {
            std::unique_lock<std::mutex> lock(m_timedQueueMtx);
            m_timedKeysQueue.emplace(path, std::chrono::steady_clock::now() + timeout);
        }
        m_cv.notify_one();
    }
}


template<typename T>
void StorageAccessor::setValueImpl(const std::string &path, const T &val, std::chrono::milliseconds timeout)
{

    std::optional<MountPoint> mounted = m_storageTree.getPriorityStorage(path);
    if (!mounted)
        throw NoSuchPathException("No suitable physical volumes for given physical_path");

    auto endianed_val = boost::endian::native_to_little(val);
    auto *data = reinterpret_cast<uint8_t *>(&endianed_val);
    std::vector<uint8_t> data_vec;
    data_vec.assign(data, data + sizeof(endianed_val));

    mounted.value().storage->setValue(mounted.value().fullPath(path), data_vec);
}

template<>
void StorageAccessor::setValueImpl<std::vector<uint8_t>>(const std::string &path, const std::vector<uint8_t> &vec, std::chrono::milliseconds timeout)
{

    std::optional<MountPoint> mounted = m_storageTree.getPriorityStorage(path);
    if (!mounted)
        throw NoSuchPathException("No suitable physical volumes for given physical_path");

    mounted.value().storage->setValue(mounted.value().fullPath(path), vec);
}


template<>
void StorageAccessor::setValue<std::vector<uint8_t>>(const std::string &path, const std::vector<uint8_t> &val, std::chrono::milliseconds timeout)
{
    setValueImpl(path, val, timeout);
}


template<>
void StorageAccessor::setValueImpl<std::string>(const std::string &path, const std::string &str, std::chrono::milliseconds timeout)
{

    std::optional<MountPoint> mounted = m_storageTree.getPriorityStorage(path);
    if (!mounted)
        throw NoSuchPathException("No suitable physical volumes for given physical_path");

    auto endianed_val = boost::endian::native_to_little(str);
    auto *mem = reinterpret_cast<const uint8_t *>(endianed_val.c_str());
    std::vector<uint8_t> data_vec;
    data_vec.assign(mem, mem + endianed_val.size());
    data_vec.push_back('\0');

    mounted.value().storage->setValue(mounted.value().fullPath(path), data_vec);
}


template<typename T>
T StorageAccessor::getValue(const std::string &path)
{
    using namespace TypeTraits;
    std::set<MountPoint> storages = m_storageTree.getSuitableStorageList(path);
    for (const auto &mount_point : storages) {
        try {
            auto data_vector = mount_point.storage->getValue(mount_point.fullPath(path));

            if constexpr (has_deserialize_method<T>::value) {
                return T::deserialize(data_vector);
            } else if constexpr (has_deserialize_spec<T>::value) {
                return deserialize<T>(data_vector);
            } else {
                auto endianed_val = reinterpret_cast<const T *>(data_vector.data());
                return boost::endian::little_to_native(*endianed_val);
            }
        }
        catch (const NoSuchPathException &ex) {}
    }
    throw NoSuchPathException("Path not found");
}


template<>
std::string StorageAccessor::getValue<std::string>(const std::string &path)
{

    std::set<MountPoint> storages = m_storageTree.getSuitableStorageList(path);
    for (const auto &mount_point : storages) {
        try {
            auto blob = mount_point.storage->getValue(mount_point.fullPath(path));

            if (blob.back() != '\0')
                throw BadValueException("No NULL-terminator in data!"); // avoid UB

            auto endianed_val = reinterpret_cast<const char *>(blob.data());
            return std::string(boost::endian::little_to_native(endianed_val));
        }
        catch (const NoSuchPathException &ex) {}
    }
    throw NoSuchPathException("Path not found");
}

template<>
std::vector<uint8_t> StorageAccessor::getValue<std::vector<uint8_t>>(const std::string &path)
{

    std::set<MountPoint> storages = m_storageTree.getSuitableStorageList(path);
    for (const auto &mount_point : storages) {
        try {
            return mount_point.storage->getValue(mount_point.fullPath(path));
        }
        catch (const NoSuchPathException &ex) {}
    }
    throw NoSuchPathException("Path not found");
}

void StorageAccessor::mountPhysicalVolume(std::shared_ptr<IStorage> storage, const std::string &mount_point,
                                          size_t priority, const std::string &path)
{
    m_storageTree.addMountPoint(mount_point, storage, path, priority);
}


void StorageAccessor::removeValue(const std::string &path)
{
    std::set<MountPoint> storages = m_storageTree.getSuitableStorageList(path);
    for (const auto &mount_point : storages) {
        try {
            mount_point.storage->removeValue(mount_point.fullPath(path));
        }
        catch (const NoSuchPathException &ex) {}
    }
}


void StorageAccessor::umountVolume(const std::string &path)
{
    m_storageTree.removeMountPoint(path);
}


void StorageAccessor::timedKeyDestroyerThread()
{
    while (!m_exit) {
        std::unique_lock<std::mutex> lock(m_timedQueueMtx);
        if (m_timedKeysQueue.empty())
            m_cv.wait(lock);
        else
            m_cv.wait_until(lock, m_timedKeysQueue.top().timeout);

        if (m_exit)
            break;

        if (m_timedKeysQueue.empty())
            continue;

        auto now = std::chrono::steady_clock::now();
        while (!m_timedKeysQueue.empty() && now > m_timedKeysQueue.top().timeout) {
            auto key = m_timedKeysQueue.top().path;
            m_timedKeysQueue.pop();
            removeValue(key);
        }
    }
}


StorageAccessor::StorageAccessor() : m_destroyerThread(std::thread(&StorageAccessor::timedKeyDestroyerThread, this))
{
}


StorageAccessor::~StorageAccessor()
{
    m_exit = true;
    m_cv.notify_one();
    m_destroyerThread.join();
};


}

#endif // UNIVERSALSTORAGE_STORAGEACCESSOR_H
