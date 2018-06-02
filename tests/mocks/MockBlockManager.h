//
// Created by maxon on 27.05.18.
//

#ifndef UNIVERSALSTORAGE_MOCKBLOCKMANAGER_H
#define UNIVERSALSTORAGE_MOCKBLOCKMANAGER_H

#include "IBlockManager.h"
#include <gmock/gmock.h>

using namespace UniversalStorage;

class MockBlockManager : public IBlockManager
{
public:
    MOCK_METHOD0(getFreeBlock, uint8_t*());
    MOCK_METHOD0(getFreeTreeNodeBlock, uint8_t*());
    MOCK_METHOD0(getRootBlock, uint8_t*());
    MOCK_METHOD1(getBlock, uint8_t*(uint64_t offset));
    MOCK_METHOD1(getOffset, uint64_t(uint8_t *ptr));
    MOCK_METHOD1(getPathString, std::string(uint64_t offset));

//    virtual uint8_t *getFreeBlock() = 0;
//    virtual uint8_t *getFreeTreeNodeBlock() = 0;
//    virtual uint8_t *getRootBlock() = 0;
//    virtual uint8_t *getBlock(uint64_t offset) = 0;
//    virtual uint64_t getOffset(uint8_t *ptr) = 0;
};
#endif //UNIVERSALSTORAGE_MOCKBLOCKMANAGER_H
