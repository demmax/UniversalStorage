//
// Created by maxon on 26.05.18.
//

#ifndef UNIVERSALSTORAGE_PHYSICALSTORAGE_H
#define UNIVERSALSTORAGE_PHYSICALSTORAGE_H

#include "IStorage.h"
#include "BPTree.h"
#include "IBlockManager.h"


namespace UniversalStorage {


class PhysicalStorage : public IStorage
{
public:
    void setValue(const std::string &path, const std::vector<uint8_t> &data) override;
    std::vector<uint8_t> getValue(const std::string &path) const override;
    void removeValue(const std::string &path) override;
    bool isExist(const std::string &path) const override;

protected:
    uint8_t *storeData(const std::vector<uint8_t> &data, uint64_t offset);
    uint64_t storeDataInNewBlock(const uint8_t *data, size_t data_size);
    std::string getStringFromPathOffset(uint64_t offset) const;
    std::vector<uint8_t> getDataFromOffset(uint64_t offset) const;
    uint64_t hash(const std::string &str) const;
    uint64_t packVector(const std::vector<uint8_t> &vec) const;
    std::vector<uint8_t> unpackValue(uint64_t data) const;


    BPTree m_btree;
    IBlockManagerPtr m_blockManager;
};

}

#endif //UNIVERSALSTORAGE_PHYSICALSTORAGE_H
