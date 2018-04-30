//
// Created by maxon on 29.04.18.
//

#ifndef UNIVERSALSTORAGE_IPHYSICALSTORAGE_H
#define UNIVERSALSTORAGE_IPHYSICALSTORAGE_H

#include <any>
#include <memory>
#include <cstring>
#include <vector>


class IPhysicalStorage
{
public:
    virtual void setValue(const std::string &path, const uint8_t* data, size_t size) = 0;
    virtual std::tuple<uint8_t*, size_t> getValue(const std::string &path) const = 0;
    virtual bool isExist(const std::string &path) const = 0;
    virtual ~IPhysicalStorage() = default;
};


using IPhysicalStoragePtr = std::shared_ptr<IPhysicalStorage>;


#endif //UNIVERSALSTORAGE_IPHYSICALSTORAGE_H
