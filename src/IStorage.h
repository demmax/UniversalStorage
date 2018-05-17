//
// Created by maxon on 29.04.18.
//

#ifndef UNIVERSALSTORAGE_ISTORAGE_H
#define UNIVERSALSTORAGE_ISTORAGE_H

#include <any>
#include <memory>
#include <cstring>
#include <vector>

namespace UniversalStorage {

class IStorage
{
public:
    virtual void setValue(const std::string &path, const std::vector<uint8_t> &data) = 0;
    virtual std::vector<uint8_t> getValue(const std::string &path) const = 0;
    virtual void removeValue(const std::string &path) = 0;
    virtual bool isExist(const std::string &path) const = 0;
    virtual ~IStorage() = default;
};


using IStoragePtr = std::shared_ptr<IStorage>;

}

#endif // UNIVERSALSTORAGE_ISTORAGE_H
