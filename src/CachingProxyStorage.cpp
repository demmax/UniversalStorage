//
// Created by maxon on 06.05.18.
//

#include "CachingProxyStorage.h"
#include "exceptions.h"

#include <boost/format.hpp>


using namespace UniversalStorage;

CachingProxyStorage::CachingProxyStorage(IStoragePtr storage, size_t max_size)
        : p_storage(std::move(storage)), m_maxSize(max_size), m_currentSize(0)
{
}

void CachingProxyStorage::setValue(const std::string &path, const std::vector<uint8_t> &data)
{
    p_storage->setValue(path, data);

    auto it = m_cache.find(path);
    if (it == m_cache.end()) {
        freeCache(data.size());

        auto [iter, success] = m_cache.insert(std::make_pair(path, CachedData{data, std::any()}));
        if (success) {
            m_currentSize += data.size();
            m_lru_list.push_back(iter);
            iter->second.queue_iterator = --m_lru_list.end();
        }
    } else {
        size_t old_size = it->second.data.size();
        if (data.size() > old_size)
            freeCache(data.size() - old_size);
        it->second.data = data;
        m_currentSize = m_currentSize - old_size + data.size();
        advanceIterator(it);
    }
}

std::vector<uint8_t> CachingProxyStorage::getValue(const std::string &path) const
{
    auto it = m_cache.find(path);
    if (it != m_cache.end()) {
        advanceIterator(it);
        return it->second.data;
    }

    auto data = p_storage->getValue(path);
    freeCache(data.size());
    auto [iter, success] = m_cache.insert(std::make_pair(path, CachedData{data, std::any()}));
    if (success) {
        m_currentSize += data.size();
        m_lru_list.push_back(iter);
        iter->second.queue_iterator = --m_lru_list.end();
    }

    return data;
}

void CachingProxyStorage::advanceIterator(Map::iterator iter) const
{
    m_lru_list.splice(m_lru_list.end(), m_lru_list, std::any_cast<IteratorList::iterator>(iter->second.queue_iterator));
}

bool CachingProxyStorage::isExist(const std::string &path) const
{
    return false;
}

void CachingProxyStorage::freeCache(size_t additional_space) const
{
    while (m_maxSize < m_currentSize + additional_space) {
        if (m_lru_list.empty())
            throw BadValueException("Not enough cache size to store elements");
        auto to_del = m_lru_list.front();
        m_lru_list.pop_front();
        size_t diff = to_del->second.data.size();
        m_cache.erase(to_del);
        m_currentSize -= diff;
    }
}

void CachingProxyStorage::removeValue(const std::string &path)
{
    p_storage->removeValue(path);
    auto it = m_cache.find(path);
    if (it != m_cache.end()) {
        auto list_iter = std::any_cast<IteratorList::iterator>(it->second.queue_iterator);
        m_cache.erase(it);
        m_lru_list.erase(list_iter);
    }
}

