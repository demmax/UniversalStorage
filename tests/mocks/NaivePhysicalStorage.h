//
// Created by maxon on 30.04.18.
//

#ifndef UNIVERSALSTORAGE_NAIVEPHYSICALSTORAGE_H
#define UNIVERSALSTORAGE_NAIVEPHYSICALSTORAGE_H

#include "IPhysicalStorage.h"

#include <unordered_map>
#include <vector>


class NaivePhysicalStorage : public IPhysicalStorage
{
public:
    void setValue(const std::string &path, const uint8_t* data, size_t size) override;
    std::tuple<uint8_t*, size_t> getValue(const std::string &path) const override;
    bool isExist(const std::string &path) const override;
    ~NaivePhysicalStorage() {}

protected:
    std::unordered_map<std::string, std::vector<uint8_t>> m_dataMap;
};


#endif //UNIVERSALSTORAGE_NAIVEPHYSICALSTORAGE_H
