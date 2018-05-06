//
// Created by maxon on 06.05.18.
//

#ifndef UNIVERSALSTORAGE_CACHINGPROXYSTORAGE_H
#define UNIVERSALSTORAGE_CACHINGPROXYSTORAGE_H

#include "IStorage.h"

class CachingProxyStorage : public IStorage
{
public:
    CachingProxyStorage(IStoragePtr storage);
    void setValue(const std::string &path, const std::vector<uint8_t> &data) override;
    std::vector<uint8_t> getValue(const std::string &path) const override;
    bool isExist(const std::string &path) const override;
    ~CachingProxyStorage() override = default;

protected:
    IStoragePtr p_storage;
};


#endif //UNIVERSALSTORAGE_CACHINGPROXYSTORAGE_H
