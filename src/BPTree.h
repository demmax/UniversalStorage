//
// Created by maxon on 26.05.18.
//

#ifndef UNIVERSALSTORAGE_BPTREE_H
#define UNIVERSALSTORAGE_BPTREE_H

#include <memory>

#include "IBlockManager.h"


namespace UniversalStorage {


class BPTree
{
public:
    using pointer = uint8_t*;

    BPTree(IBlockManagerPtr blockManager, pointer begin);

    void addKey(uint64_t key, uint64_t data_off);
    uint64_t getData(uint64_t key);
    void removeKey(uint64_t key);

protected:
    pointer internalAddKey(uint64_t key, uint64_t data, pointer node); //!< Returns true, if nodes were splitted.
    void insertData(uint64_t key, uint64_t data, pointer data_ptr);
    pointer splitWithSibling(pointer node, uint64_t key, uint64_t data); //!< Returns new sibling
    uint8_t findPosition(uint64_t key, pointer data_ptr, uint8_t data_count);
    pointer  getPointerByOffset(uint64_t offset) const;
    uint64_t getOffsetByPointer(pointer ptr) const;
    uint64_t retrieveData(pointer data_ptr) const;
    uint64_t retrieveData(pointer node, uint8_t index) const;

    IBlockManagerPtr m_blockManager;
    pointer m_root;

    static constexpr uint64_t IS_LEAF_OFFSET = 0;
    static constexpr uint64_t CHILD_COUNT_OFFSET = 2;
    static constexpr uint64_t DATA_OFFSET = 10;
    static constexpr uint8_t  MAX_CHILD_COUNT = 100;
    static constexpr uint8_t  KEY_SIZE = 8;
    static constexpr uint8_t  DATA_SIZE = 8;
    static constexpr uint8_t  DATA_ITEM_SIZE = KEY_SIZE + DATA_SIZE;
    static constexpr uint64_t MAX_NODE_SIZE = DATA_OFFSET + (MAX_CHILD_COUNT * DATA_ITEM_SIZE);

    static constexpr uint64_t MIDDLE_POSITION = MAX_CHILD_COUNT / 2;
};

}

#endif //UNIVERSALSTORAGE_BPTREE_H
