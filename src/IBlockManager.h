//
// Created by maxon on 26.05.18.
//

#ifndef UNIVERSALSTORAGE_IBLOCKMANAGER_H
#define UNIVERSALSTORAGE_IBLOCKMANAGER_H

#include <memory>

namespace UniversalStorage {


class IBlockManager
{
public:
    virtual uint8_t *getFreeBlock() = 0;
    virtual uint8_t *getFreeTreeNodeBlock() = 0;
    virtual uint8_t *getRootBlock() = 0;
    virtual uint8_t *getBlock(uint64_t offset) = 0;
    virtual uint64_t getOffset(uint8_t *ptr) = 0;
    virtual std::string getPathString(uint64_t offset) = 0;
    virtual ~IBlockManager() = default;
};

using IBlockManagerPtr = std::shared_ptr<IBlockManager>;

}

#endif //UNIVERSALSTORAGE_IBLOCKMANAGER_H
