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
    MOCK_METHOD0(getRootBlock, uint8_t*());
};
#endif //UNIVERSALSTORAGE_MOCKBLOCKMANAGER_H
