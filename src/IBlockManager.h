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
    virtual uint8_t* getFreeBlock() = 0;
    virtual ~IBlockManager() = default;
};

using IBlockManagerPtr = std::shared_ptr<IBlockManager>;

}

#endif //UNIVERSALSTORAGE_IBLOCKMANAGER_H
