//
// Created by maxon on 30.04.18.
//

#ifndef UNIVERSALSTORAGE_MEMORYSTORAGE_H
#define UNIVERSALSTORAGE_MEMORYSTORAGE_H

#include "../../src/IStorage.h"

#include <unordered_map>
#include <vector>


class MemoryStorage : public IStorage
{
public:
    std::vector<uint8_t> getValue(const std::string &path) const override;
    void setValue(const std::string &path, const std::vector<uint8_t> &data) override;
    bool isExist(const std::string &path) const override;
    ~MemoryStorage() override = default;

protected:
    std::unordered_map<std::string, std::vector<uint8_t>> m_dataMap;
};


#endif //UNIVERSALSTORAGE_MEMORYSTORAGE_H
