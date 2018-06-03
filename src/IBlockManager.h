//
// Created by maxon on 26.05.18.
//

#ifndef UNIVERSALSTORAGE_IBLOCKMANAGER_H
#define UNIVERSALSTORAGE_IBLOCKMANAGER_H

#include <memory>
#include <vector>

namespace UniversalStorage {


class IBlockManager
{
public:
    static constexpr size_t TREE_NODE_BLOCK_SIZE = 2048;
    static constexpr size_t DATA_BLOCK_SIZE = 1024;
    static constexpr size_t DATA_BLOCKS_IN_TREE_NODE = TREE_NODE_BLOCK_SIZE / DATA_BLOCK_SIZE;


    virtual uint8_t *getRootBlock() = 0;
    virtual uint8_t *getTreeNodeBlock(uint64_t offset) = 0;
    virtual uint8_t *getFreeTreeNodeBlock() = 0;
    virtual void freeTreeNodeBlock(uint8_t *address) = 0;

    virtual uint64_t storeData(const std::vector<uint8_t> &data, uint64_t offset) = 0;
    virtual uint64_t storeNewData(const std::vector<uint8_t> &data) = 0;
    virtual uint64_t storePath(const std::string &path, uint64_t offset) = 0;
    virtual uint64_t storeNewPath(const std::string &path) = 0;
    virtual void freeBlock(uint64_t offset) = 0;

    virtual std::string getPathFromBlock(uint64_t offset) = 0;
    virtual std::vector<uint8_t> getDataFromBlock(uint64_t offset) = 0;

    virtual bool isRootInitialized() const = 0;
    virtual uint64_t getOffset(uint8_t *address) = 0;

    virtual ~IBlockManager() = default;
};

using IBlockManagerPtr = std::shared_ptr<IBlockManager>;

}

#endif //UNIVERSALSTORAGE_IBLOCKMANAGER_H
