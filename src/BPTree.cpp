//
// Created by maxon on 26.05.18.
//

#include "BPTree.h"
#include "exceptions.h"
#include <cstring>
#include <boost/endian/conversion.hpp>
#include <iostream>

using namespace UniversalStorage;

#define DEBUG_ASSERT

#ifdef DEBUG_ASSERT
#define DEBUG_ASSERT_INVARIANTS assertInvariants(m_root)
#else
#define DEBUG_ASSERT_INVARIANTS
#endif



BPTree::BPTree(IBlockManagerPtr blockManager) : m_blockManager(std::move(blockManager))
{
    m_root = std::make_shared<BTreeNode>();
    m_root->offset = 0;
}


BPTree::~BPTree()
{
}


void BPTree::clearNode(BTreeNodePtr node)
{
}


void BPTree::addKey(uint64_t key, uint64_t data, uint64_t path_off, bool is_data)
{
    data_type real_data{.data = data, .path_offset = path_off, .is_data = is_data};
    auto sibling = internalAddData(m_root, key, real_data);
    if (sibling) {  // Move root to new place & add fill it by new pointers
        auto new_node = std::make_shared<BTreeNode>();
        std::swap(*m_root, *new_node);
        m_root->is_leaf = false;

        auto max_left_key = new_node->data_vector.back().key;
        auto max_new_key = sibling->data_vector.back().key;
        m_root->data_vector.emplace_back(DataItem(max_left_key, new_node));
        m_root->data_vector.emplace_back(DataItem(max_new_key, sibling));
        new_node->right_sibling = sibling;
        sibling->left_sibling = new_node;
    }

    DEBUG_ASSERT_INVARIANTS;
}


BTreeNodePtr BPTree::internalAddData(BTreeNodePtr node, uint64_t key, data_type data)
{
    if (node->is_leaf) {
        if (node->data_vector.size() < MAX_LOAD_FACTOR - 1) {
            insertData(node, key, data);
            return nullptr;
        }
        else {
            return makeSibling(node, key, data);
        }
    }
    else {
        auto it = std::lower_bound(node->data_vector.begin(), node->data_vector.end(), key);
        if (it == node->data_vector.end())
            it = std::prev(it);
        BTreeNodePtr child = getPtr((*it).data);
        auto sibling = internalAddData(child, key, data);
        (*it).key = child->data_vector.back().key;

        if (!sibling) {
            return nullptr;
        }

        // Children was splitted. Need add new child & update values
        auto max_left_key = child->data_vector.back().key;
        auto max_sibling_key = sibling->data_vector.back().key;
        if (node->data_vector.size() < MAX_LOAD_FACTOR - 1) {
            (*it).key = max_left_key;
            node->data_vector.insert(std::next(it), DataItem(max_sibling_key, sibling));
//            insertData(node, max_sibling_key, sibling);
            return nullptr;
        }
        else { // Can't add. Need splitting too.
            return makeSibling(node, max_sibling_key, sibling);
        }
    }
}



void BPTree::insertData(BTreeNodePtr node, uint64_t key, const std::any &data)
{
    node->data_vector.insert(
            std::lower_bound(node->data_vector.begin(), node->data_vector.end(), key),
            DataItem(key, data)
    );
}


BTreeNodePtr BPTree::makeSibling(BTreeNodePtr node, uint64_t key, const std::any &data)
{
    auto sibling = std::make_shared<BTreeNode>();
    sibling->is_leaf = node->is_leaf;
    uint8_t to_move = MAX_LOAD_FACTOR / 2;
    auto index = std::distance(node->data_vector.begin(), std::lower_bound(node->data_vector.begin(), node->data_vector.end(), key));
    if (index < to_move)
        --to_move;

    sibling->data_vector.insert(
            sibling->data_vector.end(),
            std::make_move_iterator(node->data_vector.begin() + to_move),
            std::make_move_iterator(node->data_vector.end())
    );
    node->data_vector.erase(node->data_vector.begin() + to_move, node->data_vector.end());
    insertData(index >= to_move ? sibling : node, key, data);
    if (node->right_sibling) {
        node->right_sibling->left_sibling = sibling;
        sibling->right_sibling = node->right_sibling;
    }
    sibling->left_sibling = node;
    node->right_sibling = sibling;
    return sibling;
}


std::vector<data_type> BPTree::getValues(uint64_t key) const
{
    BTreeNodePtr node = getChildWithKey(m_root, key);
    if (node->is_leaf) {
        auto it = std::find_if(node->data_vector.begin(), node->data_vector.end(), [&](auto data) { return data.key == key; });
        if (it == node->data_vector.end())
            throw NoSuchPathException(std::to_string(key).c_str());

        std::vector<data_type> result;
        result.push_back(getData((*it).data));
        while ((it = std::next(it)) != node->data_vector.end() && (*it).key == key) {
            result.push_back(getData((*it).data));
        }


        if (it == node->data_vector.end()) { // Need check sibling
            while (node->right_sibling) {
                node = node->right_sibling;
                it = node->data_vector.begin();
                while (it != node->data_vector.end() && (*it).key == key) {
                    result.push_back(getData((*it).data));
                    ++it;
                }

                if (it != node->data_vector.end())
                    break;
            }
        }
        return result;
    }
    throw StorageException((std::string(__func__) + "Something goes completely wrong").c_str());
}


void BPTree::removeKey(uint64_t key, uint64_t path_offset)
{
    RemoveStatus status = internalRemoveData(m_root, key, path_offset);
    DEBUG_ASSERT_INVARIANTS;
    if (status == NEED_SIBLING)
        throw NoSuchPathException((std::to_string(key) + " (" + std::to_string(path_offset) +")").c_str());
}


BTreeNodePtr BPTree::getChildWithKey(BTreeNodePtr node, uint64_t key) const
{
    auto it = std::lower_bound(node->data_vector.begin(), node->data_vector.end(), key);
    if (it == node->data_vector.end())
        throw NoSuchPathException(std::to_string(key).c_str());

    if (node->is_leaf)
        return node;

    auto child = getPtr((*it).data);
    return getChildWithKey(child, key);
}


BPTree::RemoveStatus BPTree::internalRemoveData(BTreeNodePtr node, uint64_t key, uint64_t path_offset)
{
    if (node->is_leaf) {
        auto del_it = std::find_if(node->data_vector.begin(), node->data_vector.end(), [&](auto data) { return data.key == key; });
        if (del_it == node->data_vector.end())
            throw NoSuchPathException((std::to_string(key) + " (" + std::to_string(path_offset) +")").c_str());

        bool is_removed = false;
        // Keys may be same, so need search in siblings too.
        while (del_it != node->data_vector.end()) {
            if (getData((*del_it).data).path_offset == path_offset) {
                node->data_vector.erase(del_it);
                is_removed = true;
                break;
            }

            if (++del_it != node->data_vector.end() && (*del_it).key != key)
                throw NoSuchPathException((std::to_string(key) + " (" + std::to_string(path_offset) +")").c_str());
        }

        if (!is_removed)
            return NEED_SIBLING;

//        if (del_it == node->data_vector.end()) {
//            if (node->data_vector.empty()) // Last element case.
//                return REMOVE_OK;
//            return NEED_SIBLING;
//        }

        if (node->data_vector.size() >= MIN_LOAD_FACTOR) {
            return REMOVE_OK;
        }
        else { // Need merge with sibling
            return mergeWithSibling(node);
        }
    }
    else { // Not a leaf. Search suitable subtree.
        auto child_it = std::lower_bound(node->data_vector.begin(), node->data_vector.end(), key);
        if (child_it == node->data_vector.end())
            throw NoSuchPathException((std::to_string(key) + " (" + std::to_string(path_offset) +")").c_str());

        BTreeNodePtr child;
        RemoveStatus status;

        do {
            child = getPtr((*child_it).data);
            status = internalRemoveData(child, key, path_offset);
            if (status == NEED_SIBLING && ++child_it == node->data_vector.end())
                return NEED_SIBLING;
        } while (status == NEED_SIBLING);

        if (status == REMOVE_OK) {
            (*child_it).key = child->data_vector.back().key;
            return REMOVE_OK;
        }

        if (status == REMOVE_MERGED) {
            if (node->left_sibling != node->left_sibling->data_vector.empty()) {
                node->data_vector.erase(std::prev(child_it));
            }
            else { // Definitely exist, because successfully merged
                node->data_vector.erase(std::next(child_it));
            }

            if (node->data_vector.size() >= MIN_LOAD_FACTOR) {
                return REMOVE_OK;
            }
            else {
                return mergeWithSibling(node);
            }
        }

        if (status == CANT_MERGE) { // Subtree empty. Move child up.
            std::swap(*node, *child);
            return CANT_MERGE;
        }
    }

    throw StorageException((std::string(__func__) + ":  something goes completely wrong (" + std::to_string(key) + ")").c_str());
}


BPTree::RemoveStatus BPTree::mergeWithSibling(BTreeNodePtr node)
{
    BTreeNodePtr sibling = nullptr;
    if (node->left_sibling && node->left_sibling->data_vector.size() > MIN_LOAD_FACTOR) {
        sibling = node->left_sibling;
    }
    else if (node->right_sibling) {
        sibling = node->right_sibling;
    }

    if (sibling == nullptr)
        return CANT_MERGE;

    if (sibling->data_vector.size() > MIN_LOAD_FACTOR) {
        node->data_vector.push_back(sibling->data_vector.front());
        sibling->data_vector.erase(sibling->data_vector.begin());
        return REMOVE_OK;
    }
    else { // Sibling hasn't enough nodes too. Merge them.
        std::copy(sibling->data_vector.begin(), sibling->data_vector.end(), std::back_inserter(node->data_vector));
        if (sibling == node->left_sibling) {
            node->left_sibling = sibling->left_sibling;
            if (node->left_sibling)
                node->left_sibling->right_sibling = node;
        }
        else {
            node->right_sibling = sibling->right_sibling;
            if (node->right_sibling)
                node->right_sibling->left_sibling = node;
        }
        return REMOVE_MERGED;
    }
}


void BPTree::assertInvariants(BTreeNodePtr node)
{
    if (node != m_root) {
        assert(node->data_vector.size() >= MIN_LOAD_FACTOR && node->data_vector.size() < MAX_LOAD_FACTOR);
    }
    else {
        assert(node->data_vector.size() >= 0 && node->data_vector.size() < MAX_LOAD_FACTOR);
        if (!node->data_vector.empty()) {
            auto child = getChildWithKey(m_root, 0);
            assert(!child->left_sibling);
            child = getChildWithKey(m_root, m_root->data_vector.back().key);
            assert(!child->right_sibling);
        }
    }

    for (auto i = 1u; i < node->data_vector.size(); ++i) {
        assert(node->data_vector[i].key >= node->data_vector[i - 1].key);
        if (!node->is_leaf) {
            auto child = getPtr(node->data_vector[i].data).get();
            assert(child->data_vector.back().key == node->data_vector[i].key);
        }
    }

    if (!node->is_leaf) {
        auto old_child = getPtr(node->data_vector[0].data);
        assertInvariants(old_child);
        for (auto i = 1u; i < node->data_vector.size(); ++i) {
            auto child = getPtr(node->data_vector[i].data);
            assert(old_child->right_sibling == child);
            assert(child->left_sibling == old_child);
            assertInvariants(child);
            old_child = child;
        }
    }
}


data_type BPTree::getData(const std::any &var) const
{
    return std::any_cast<data_type>(var);
}


BTreeNodePtr BPTree::getPtr(const std::any &var) const
{
    return std::any_cast<BTreeNodePtr>(var);
}


//void BPTree::sync()
//{
//
//}

void BPTree::load()
{
    uint8_t* root_ptr = m_blockManager->getRootBlock();
    m_root = makeNodeFromData(root_ptr, 0);
}


BTreeNodePtr BPTree::makeNodeFromData(uint8_t *base, uint64_t offset)
{
    auto node = std::make_shared<BTreeNode>();

    uint8_t *node_ptr = base + offset;
    node->is_leaf = node_ptr[IS_LEAF_OFFSET];
    node->offset = offset;
    uint8_t data_count = node_ptr[DATA_COUNT_OFFSET];
    uint64_t left_sibling = node_ptr[LEFT_SIBLING_OFFSET];
    uint64_t right_sibling = node_ptr[RIGHT_SIBLING_OFFSET];

    if (node->is_leaf) {
        for (auto i = 0; i < data_count; i++) {
            uint8_t *ptr = node_ptr + DATA_OFFSET + (i * IS_DATA_FLAG_SIZE);
            uint64_t key = *(reinterpret_cast<uint64_t*>(ptr));
            ptr += KEY_SIZE;
            uint64_t data = *(reinterpret_cast<uint64_t*>(ptr));
            ptr += DATA_SIZE;
            uint64_t path_offset = *(reinterpret_cast<uint64_t*>(ptr));
            ptr += PATH_OFFSET_SIZE;
            uint8_t is_data = *ptr;

            node->data_vector.emplace_back(
                    DataItem(key, RealData{.data = data, .path_offset = path_offset, .is_data = is_data})
            );
        }
    }
    else {
        for (auto i = 0; i < data_count; i++) {
            uint8_t *ptr = node_ptr + DATA_OFFSET + (i * IS_DATA_FLAG_SIZE);
            uint64_t key = *(reinterpret_cast<uint64_t*>(ptr));
            ptr += KEY_SIZE;
            uint64_t child_offset = *(reinterpret_cast<uint64_t*>(ptr));

            auto child = makeNodeFromData(base, child_offset);
            node->data_vector.emplace_back(DataItem(key, child));
        }
    }

    if (left_sibling)
        node->left_sibling  = makeNodeFromData(base, left_sibling);
    if (right_sibling)
        node->right_sibling = makeNodeFromData(base, right_sibling);

    return node;
}


void BPTree::store()
{
    storeSubtree(m_root);
}


void BPTree::storeSubtree(BTreeNodePtr node)
{
    uint8_t *node_ptr = nullptr;
    if (node->offset) {
        node_ptr = m_blockManager->getBlock(node->offset);
    }
    else {
        node_ptr = m_blockManager->getFreeTreeNodeBlock();
    }
    node_ptr[IS_LEAF_OFFSET] = static_cast<uint8_t>(node->is_leaf);
    node_ptr[DATA_COUNT_OFFSET] = static_cast<uint8_t>(node->data_vector.size());

    auto *left_sibling = reinterpret_cast<uint64_t*>(node_ptr + LEFT_SIBLING_OFFSET);
    if (node->left_sibling) {
        if (!node->left_sibling->offset) {
            node->left_sibling->offset = m_blockManager->getOffset(m_blockManager->getFreeTreeNodeBlock());
        }
        *left_sibling = boost::endian::native_to_little(node->left_sibling->offset);
    }
    else {
        *left_sibling = 0;
    }

    auto *right_sibling = reinterpret_cast<uint64_t*>(node_ptr + RIGHT_SIBLING_OFFSET);
    if (node->right_sibling) {
        if (!node->right_sibling->offset) {
            node->right_sibling->offset = m_blockManager->getOffset(m_blockManager->getFreeTreeNodeBlock());
        }
        *right_sibling = boost::endian::native_to_little(node->right_sibling->offset);
    }
    else {
        *right_sibling = 0;
    }

    if (node->is_leaf) {
        for (auto i = 0u; i < node->data_vector.size(); i++) {
            const DataItem &item = node->data_vector[i];
            const auto & data_item = getData(item.data);
            uint8_t *ptr = node_ptr + DATA_OFFSET + (i * IS_DATA_FLAG_SIZE);

            auto *key = reinterpret_cast<uint64_t *>(ptr);
            *key = boost::endian::native_to_little(item.key);
            ptr += KEY_SIZE;

            auto *data = reinterpret_cast<uint64_t *>(ptr);
            *data = boost::endian::native_to_little(data_item.data);
            ptr += DATA_SIZE;

            auto *path_offset = reinterpret_cast<uint64_t *>(ptr);
            *path_offset = boost::endian::native_to_little(data_item.path_offset);
            ptr += PATH_OFFSET_SIZE;

            *ptr = static_cast<uint8_t>(data_item.is_data);
        }
    }
    else { // Not a leaf.
        for (auto i = 0u; i < node->data_vector.size(); i++) {
            const DataItem &item = node->data_vector[i];
            auto child = getPtr(item.data);
            storeSubtree(child);

            uint8_t *ptr = node_ptr + DATA_OFFSET + (i * IS_DATA_FLAG_SIZE);

            auto *key = reinterpret_cast<uint64_t *>(ptr);
            *key = boost::endian::native_to_little(item.key);
            ptr += KEY_SIZE;

            auto *data = reinterpret_cast<uint64_t *>(ptr);
            *data = boost::endian::native_to_little(child->offset);
            ptr += DATA_SIZE;

            auto *path_offset = reinterpret_cast<uint64_t *>(ptr);
            *path_offset = 0;//boost::endian::native_to_little(data_item.path_offset);
            ptr += PATH_OFFSET_SIZE;

            *ptr = static_cast<uint8_t>(0);
        }
    }
}

