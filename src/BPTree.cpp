//
// Created by maxon on 26.05.18.
//

#include "BPTree.h"
#include "exceptions.h"
#include <cstring>
#include <boost/endian/conversion.hpp>

using namespace UniversalStorage;


BPTree::BPTree(IBlockManagerPtr blockManager, BPTree::pointer begin)
        : m_blockManager(std::move(blockManager)), m_root(begin)
{

}


void BPTree::addKey(uint64_t key, uint64_t data_off)
{
    pointer new_sibling = internalAddKey(key, data_off, m_root);
    if (new_sibling) { // Move root to new place & add fill it by new pointers
        pointer to_move = m_blockManager->getFreeBlock();
        std::memcpy(to_move, m_root, MAX_NODE_SIZE);

        uint8_t left_items_count = to_move[CHILD_COUNT_OFFSET];
        uint64_t max_left_key = retrieveData(to_move, left_items_count);
        uint8_t new_items_count  = new_sibling[CHILD_COUNT_OFFSET];
        uint64_t max_new_key  = retrieveData(new_sibling, new_items_count);


        m_root[IS_LEAF_OFFSET] = 0;
        m_root[CHILD_COUNT_OFFSET] = 2;

        // New sibling always bigger
        auto data_ptr = reinterpret_cast<uint64_t*>(m_root + DATA_OFFSET);
        *data_ptr = max_left_key; ++data_ptr;
        *data_ptr = getOffsetByPointer(to_move); ++data_ptr;
        *data_ptr = max_new_key; ++data_ptr;
        *data_ptr = getOffsetByPointer(new_sibling);
    }
}

struct TreeNode
{
    bool is_leaf;
    bool is_direct_data;
    uint8_t data_count;

    uint64_t keys_and_data;

    uint64_t data;

    uint64_t childrens; // all other place;
};


BPTree::pointer BPTree::internalAddKey(uint64_t key, uint64_t data, BPTree::pointer node)
{
    bool is_leaf = node[IS_LEAF_OFFSET];
    uint8_t &data_count = node[CHILD_COUNT_OFFSET];

    if (is_leaf) {
        if (data_count < (MAX_CHILD_COUNT - 1)) {
            insertData(key, data, node + DATA_OFFSET);
            ++data_count;
            return nullptr;
        }
        else { // Node full. Need splitting.
            return splitWithSibling(node, key, data);
        }
    }
    else {
        uint8_t  child_index = findPosition(key, node + DATA_OFFSET, data_count);
        pointer  child_key_offset_ptr = node + DATA_OFFSET + (child_index * DATA_ITEM_SIZE);
        pointer  child_offset_ptr =  child_key_offset_ptr + KEY_SIZE;
        uint64_t child_offset = retrieveData(child_offset_ptr);
        pointer  child = getPointerByOffset(child_offset);

        pointer new_sibling = internalAddKey(key, data, child);
        if (!new_sibling)
            return nullptr;

        // Children was splitted. Need add new child & update values
        uint8_t left_items_count = child[CHILD_COUNT_OFFSET];
        uint8_t new_items_count  = new_sibling[CHILD_COUNT_OFFSET];
        uint64_t max_left_key = retrieveData(child, left_items_count);
        uint64_t max_new_key  = retrieveData(new_sibling, new_items_count);

        auto *child_key = reinterpret_cast<uint64_t*>(child_key_offset_ptr);
        *child_key = boost::endian::native_to_little(max_left_key);

        if (data_count < (MAX_CHILD_COUNT - 1)) {
            pointer new_sibling_key_offset_ptr = child_key_offset_ptr + DATA_ITEM_SIZE;
            auto sibling_data = reinterpret_cast<uint64_t*>(new_sibling_key_offset_ptr);
            *sibling_data = boost::endian::native_to_little(max_new_key);
            ++sibling_data;
            *sibling_data = boost::endian::native_to_little(getOffsetByPointer(new_sibling));
        }
        else { // Can't add. Need splitting too.
            return splitWithSibling(node, max_new_key, getOffsetByPointer(new_sibling));
        }
    }
}


void BPTree::insertData(uint64_t key, uint64_t data, BPTree::pointer data_ptr)
{
    auto ptr = reinterpret_cast<uint64_t*>(data_ptr);
    *ptr = boost::endian::native_to_little(key);
    ++ptr;
    *ptr = boost::endian::native_to_little(data);
}


uint8_t BPTree::findPosition(uint64_t key, BPTree::pointer data_ptr, uint8_t data_count)
{
    for (uint8_t pos = 0; pos < data_count; pos++) {
        auto current_key = reinterpret_cast<uint64_t*>(data_ptr);
        if (key <= boost::endian::little_to_native(*current_key))
            return pos;
    }
    return data_count;
}


uint64_t BPTree::retrieveData(BPTree::pointer data_ptr) const
{
    auto data = reinterpret_cast<uint64_t*>(data_ptr);
    return boost::endian::little_to_native(*data);
}


uint64_t BPTree::retrieveData(BPTree::pointer node, uint8_t index) const
{
    return retrieveData(node + DATA_OFFSET + (index * DATA_ITEM_SIZE));
}


BPTree::pointer BPTree::splitWithSibling(BPTree::pointer node, uint64_t key, uint64_t data)
{
    uint8_t &data_count = node[CHILD_COUNT_OFFSET];
    pointer sibling = m_blockManager->getFreeBlock();
    sibling[IS_LEAF_OFFSET] = node[IS_LEAF_OFFSET];

    uint8_t pos_to_insert = findPosition(key, node + DATA_OFFSET, data_count);
    uint8_t move_position = MIDDLE_POSITION;
    bool is_data_in_sibling = pos_to_insert >= MIDDLE_POSITION;
    if (is_data_in_sibling) { // New data will in sibling. Move smaller part.
        ++move_position;
    }
    uint8_t move_bytes_count = (MAX_CHILD_COUNT - move_position) * DATA_ITEM_SIZE;
    std::memmove(sibling + DATA_OFFSET, node + DATA_OFFSET + move_position, move_bytes_count);

    insertData(key, data, is_data_in_sibling ? sibling : node);
    data_count = MIDDLE_POSITION;

    return sibling;
}


BPTree::pointer BPTree::getPointerByOffset(uint64_t offset) const
{
    return m_root + offset;
}


uint64_t BPTree::getOffsetByPointer(BPTree::pointer ptr) const
{
    return ptr - m_root;
}
