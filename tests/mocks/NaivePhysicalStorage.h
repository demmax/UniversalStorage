//
// Created by maxon on 30.04.18.
//

#ifndef UNIVERSALSTORAGE_NAIVEPHYSICALSTORAGE_H
#define UNIVERSALSTORAGE_NAIVEPHYSICALSTORAGE_H

#include "../../src/IStorage.h"

#include <unordered_map>
#include <vector>


class NaivePhysicalStorage : public IStorage
{
public:
    std::vector<uint8_t> getValue(const std::string &path) const override;
    void setValue(const std::string &path, const std::vector<uint8_t> &data) override;
    bool isExist(const std::string &path) const override;
    ~NaivePhysicalStorage() override = default;

protected:
    std::unordered_map<std::string, std::vector<uint8_t>> m_dataMap;
};


#endif //UNIVERSALSTORAGE_NAIVEPHYSICALSTORAGE_H
