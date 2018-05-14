//
// Created by maxon on 06.05.18.
//

#ifndef UNIVERSALSTORAGE_CACHINGPROXYSTORAGE_H
#define UNIVERSALSTORAGE_CACHINGPROXYSTORAGE_H

#include "IStorage.h"

#include <unordered_map>
#include <list>


class CachingProxyStorage : public IStorage
{
public:
    // TODO: Check exception-safety
    CachingProxyStorage(IStoragePtr storage, size_t max_size);
    void setValue(const std::string &path, const std::vector<uint8_t> &data) override;
    std::vector<uint8_t> getValue(const std::string &path) const override;
    void removeValue(const std::string &path) override;

    bool isExist(const std::string &path) const override;

protected:
    // TODO: change std::any to pointer.
    struct CachedData
    {
        std::vector<uint8_t> data;
        std::any queue_iterator;
    };
    using Map = std::unordered_map<std::string, CachedData>;
    using IteratorList = std::list<Map::iterator>;

    void freeCache(size_t additional_space) const;
    void advanceIterator(Map::iterator iter) const;


    // TODO: Check erasing from list. Probably, need std::remove?
    IStoragePtr p_storage;
    size_t m_maxSize;
    mutable size_t m_currentSize;
    mutable Map m_cache;
    mutable IteratorList m_lru_list;
};


#endif //UNIVERSALSTORAGE_CACHINGPROXYSTORAGE_H
