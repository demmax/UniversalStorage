//
// Created by maxon on 02.06.18.
//

#ifndef UNIVERSALSTORAGE_STUBBLOCKMANAGER_H
#define UNIVERSALSTORAGE_STUBBLOCKMANAGER_H

#include <map>
#include <random>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <queue>
#include "IBlockManager.h"
#include "utils.hpp"

class StubBlockManager : public UniversalStorage::IBlockManager
{
protected:
    std::map<uint64_t, std::string> pathMap;
    std::map<uint64_t, std::vector<uint8_t>> dataMap;
    std::map<uint64_t, std::vector<uint8_t>> nodesMap;
    std::unique_ptr<uint8_t[]> rootBlock {std::make_unique<uint8_t[]>(4096)};
    bool isRootInit { false };
    std::random_device rd;
    std::mt19937 mt;
    std::uniform_int_distribution<uint64_t> random_dist;

public:
    StubBlockManager() : mt(rd()), random_dist(0, std::numeric_limits<uint64_t>::max())
    {
    }

    uint8_t *getRootBlock() override
    {
        return rootBlock.get();
    }


    uint8_t *getTreeNodeBlock(uint64_t offset) override
    {
        if (!offset)
            return getRootBlock();
        return reinterpret_cast<uint8_t*>(offset);
    }


    uint8_t *getFreeTreeNodeBlock() override
    {
        std::vector <uint8_t> vec;
        vec.resize(4096);
        uint64_t offset = getOffset(vec.data());
        nodesMap.insert(std::make_pair(offset, vec));
        return nodesMap[offset].data();
    }


    uint64_t getOffset(uint8_t *address) override
    {
        return reinterpret_cast<uint64_t >(address);
    }

    void freeTreeNodeBlock(uint8_t *address) override
    {
        nodesMap.erase(getOffset(address));
    }


    uint64_t storeData(const std::vector<uint8_t> &data, uint64_t offset) override
    {
        dataMap[offset] = data;
        return offset;
    }


    uint64_t storeNewData(const std::vector<uint8_t> &data) override
    {
        uint64_t offset = random_dist(mt);
        dataMap.insert(std::make_pair(offset, data));
        return offset;
    }


    uint64_t storePath(const std::string &path, uint64_t offset) override
    {
        pathMap[offset] = path;
        return offset;
    }


    uint64_t storeNewPath(const std::string &path) override
    {
        uint64_t offset = random_dist(mt);
        pathMap[offset] = path;
        return offset;
    }


    std::string getPathFromBlock(uint64_t offset) override
    {
        return pathMap[offset];
    }


    std::vector<uint8_t> getDataFromBlock(uint64_t offset) override
    {
        return dataMap[offset];
    }


    void freeBlock(uint64_t offset) override
    {
        pathMap.erase(offset);
        dataMap.erase(offset);
    }

    virtual bool isRootInitialized() const override
    {
        for (auto i = 0u; i < 2048; ++i) {
            if (rootBlock[i])
                return true;
        }
        return false;
    }
};


#endif //UNIVERSALSTORAGE_STUBBLOCKMANAGER_H
