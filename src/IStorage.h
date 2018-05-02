//
// Created by maxon on 29.04.18.
//

#ifndef UNIVERSALSTORAGE_ISTORAGE_H
#define UNIVERSALSTORAGE_ISTORAGE_H

#include <any>
#include <memory>
#include <cstring>
#include <vector>


class IStorage
{
public:
    virtual void setValue(const std::string &path, const uint8_t* data, size_t size) = 0;
    virtual std::tuple<uint8_t*, size_t> getValue(const std::string &path) const = 0;
    virtual bool isExist(const std::string &path) const = 0;
    virtual ~IStorage() = default;
};


using IStoragePtr = std::shared_ptr<IStorage>;


#endif // UNIVERSALSTORAGE_ISTORAGE_H
