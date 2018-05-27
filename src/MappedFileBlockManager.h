//
// Created by maxon on 26.05.18.
//

#ifndef UNIVERSALSTORAGE_MAPPEDFILEBLOCKMANAGER_H
#define UNIVERSALSTORAGE_MAPPEDFILEBLOCKMANAGER_H

#include "IBlockManager.h"


namespace UniversalStorage {


class MappedFileBlockManager : public IBlockManager
{
public:
    uint8_t *getRootBlock() override;
    uint8_t *getFreeBlock() override;
};

}

#endif //UNIVERSALSTORAGE_MAPPEDFILEBLOCKMANAGER_H
