//
// Created by maxon on 26.05.18.
//

#include "BPTree.h"
#include "exceptions.h"
#include "utils.hpp"
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



BPTree::BPTree(IBlockManagerPtr blockManager) : m_blockManager(blockManager)
{
    load();
}


BPTree::~BPTree()
{
}


void BPTree::setValue(const std::string &path, const std::vector<uint8_t> &data)
{
    uint64_t path_hash = hash(path);
    bool is_data = false;
    uint64_t data_bytes = 0;
    if (data.size() > 8) {
        data_bytes = m_blockManager->storeNewData(data);
    }
    else {
        data_bytes = packVector(data);
        is_data = true;
    }

    if (!updateValue(path_hash, path, data_bytes, is_data)) {
        uint64_t path_offset = m_blockManager->storeNewPath(path);
        addData(path_hash, path_offset, data_bytes, is_data);
    }

    DEBUG_ASSERT_INVARIANTS;
    store();
}


bool BPTree::internalAddData(BTreeNodePtr node, uint64_t key, DataRecord data)
{
    if (node->is_leaf) {
        insertData(node, key, data);
        return true;
    }

    // Find subtree to insert
    auto it = std::lower_bound(node->data_vector.begin(), node->data_vector.end(), key);
    if (it == node->data_vector.end())
        it = std::prev(it);
    BTreeNodePtr child = getPtr((*it).data);


    // Insert data & check tree-invariants
    bool is_need_checking = internalAddData(child, key, data);
    (*it).key = child->data_vector.back().key;
    if (!is_need_checking || child->data_vector.size() <= MAX_LOAD_FACTOR)
        return false;

    // Node full. Need splitting
    auto sibling = makeSibling(child);
    auto max_left_key = child->data_vector.back().key;
    auto max_sibling_key = sibling->data_vector.back().key;
    (*it).key = max_left_key;
    node->data_vector.insert(std::next(it), DataItem(max_sibling_key, sibling));
    return true;
}



void BPTree::insertData(BTreeNodePtr node, uint64_t key, const std::any &data)
{
    node->data_vector.insert(
            std::lower_bound(node->data_vector.begin(), node->data_vector.end(), key),
            DataItem(key, data)
    );
}


BTreeNodePtr BPTree::makeSibling(BTreeNodePtr node)
{
    auto sibling = std::make_shared<BTreeNode>();
    sibling->is_leaf = node->is_leaf;
    uint8_t to_move = MAX_LOAD_FACTOR / 2;
    sibling->data_vector.insert(
            sibling->data_vector.end(),
            std::make_move_iterator(node->data_vector.begin() + to_move),
            std::make_move_iterator(node->data_vector.end())
    );
    node->data_vector.erase(node->data_vector.begin() + to_move, node->data_vector.end());
    if (node->right_sibling) {
        node->right_sibling->left_sibling = sibling;
        sibling->right_sibling = node->right_sibling;
    }
    sibling->left_sibling = node;
    node->right_sibling = sibling;
    return sibling;
}


std::vector<uint8_t> BPTree::getValue(const std::string &path) const
{
    uint64_t key = hash(path);
    BTreeNodePtr node = getChildWithKey(m_root, key);
    if (!node)
        throw NoSuchPathException(path.c_str());

    auto it = std::find_if(node->data_vector.begin(), node->data_vector.end(), [&](auto data) { return data.key == key; });
    if (it == node->data_vector.end())
        throw NoSuchPathException(path.c_str());

    while ((*it).key == key) {
        // Check path
        DataRecord data_item = getData((*it).data);
        auto p = m_blockManager->getPathFromBlock(data_item.path_offset);
        if (p == path) {
            if (data_item.is_data)
                return unpackValue(data_item.data);
            else
                return m_blockManager->getDataFromBlock(data_item.data);
        }

        it = std::next(it);
        if (it == node->data_vector.end()) { // Go to sibling
            if (!node->right_sibling)
                throw NoSuchPathException(path.c_str());

            node = node->right_sibling;
            it = node->data_vector.begin();
        }
    }

    throw NoSuchPathException(path.c_str());
}

bool BPTree::updateValue(uint64_t key, const std::string &path, uint64_t data, bool is_data)
{
    BTreeNodePtr node = getChildWithKey(m_root, key);
    if (!node)
        return false;

    auto it = std::find_if(node->data_vector.begin(), node->data_vector.end(), [&](auto data) { return data.key == key; });
    if (it == node->data_vector.end())
        return false;

    while ((*it).key == key) {
        // Check path
        DataRecord data_item = getData((*it).data);
        if (m_blockManager->getPathFromBlock(data_item.path_offset) == path) {
            if (!data_item.is_data)
                m_blockManager->freeBlock(data_item.data);
            (*it).data = DataRecord{.data = data, .path_offset = data_item.path_offset, .is_data = is_data};
            return true;
        }

        it = std::next(it);
        if (it == node->data_vector.end()) { // Go to sibling
            if (!node->right_sibling)
                return false;

            node = node->right_sibling;
            it = node->data_vector.begin();
        }
    }

    return false;
}

void BPTree::removeKey(const std::string &path)
{
    auto key = hash(path);
    internalRemoveData(m_root, key, path);
    store();
    DEBUG_ASSERT_INVARIANTS;
}


BTreeNodePtr BPTree::getChildWithKey(BTreeNodePtr node, uint64_t key) const
{
    auto it = std::lower_bound(node->data_vector.begin(), node->data_vector.end(), key);
    if (it == node->data_vector.end())
        return BTreeNodePtr();

    if (node->is_leaf)
        return node;

    auto child = getPtr((*it).data);
    return getChildWithKey(child, key);
}


BPTree::RemoveStatus BPTree::internalRemoveData(BTreeNodePtr node, uint64_t key, const std::string &path)
{
    if (node->is_leaf) {
        auto del_it = std::find_if(node->data_vector.begin(), node->data_vector.end(),
                                   [&](auto data) { return data.key == key; });
        if (del_it == node->data_vector.end())
            throw NoSuchPathException(path.c_str());

        for ( ; del_it != node->data_vector.end(); ++del_it) {
            if ((*del_it).key != key)
                throw NoSuchPathException(path.c_str());

            const DataRecord &record = getData((*del_it).data);
            if (path == m_blockManager->getPathFromBlock(record.path_offset)) {
                m_blockManager->freeBlock(record.path_offset);
                if (!record.is_data)
                    m_blockManager->freeBlock(record.data);
                node->data_vector.erase(del_it);
                return REMOVED;
            }
        }
        return NOT_FOUND; // Need search in sibling
    }

    // Not a leaf. Find subtree to remove
    auto child_it = std::lower_bound(node->data_vector.begin(), node->data_vector.end(), key);
    if (child_it == node->data_vector.end())
        throw NoSuchPathException(path.c_str());

    auto child = getPtr((*child_it).data);
    RemoveStatus status = internalRemoveData(child, key, path);

    while (status == NOT_FOUND && child->right_sibling) {
        child = child->right_sibling;
        if (child->data_vector.front().key != key)
            throw NoSuchPathException(path.c_str());

        status = internalRemoveData(child, key, path);
    }

    if (status == NOT_FOUND) { // Already iterated over all children.
        throw NoSuchPathException(path.c_str());
    }

    if (status == OK) {
        (*child_it).key = child->data_vector.back().key;
        return OK;
    }

    if (status == REMOVED) {
        if (child->data_vector.size() >= MIN_LOAD_FACTOR) {
            (*child_it).key = child->data_vector.back().key;
            return REMOVED;
        }

        // Not enough data. Merge with sibling.
        auto sibling_it = child_it;
        if (child_it != node->data_vector.begin() && getPtr((*std::prev(child_it)).data)->data_vector.size() > MIN_LOAD_FACTOR) {
            sibling_it = std::prev(child_it);
        }
        else if (child_it != std::prev(node->data_vector.end()) && getPtr((*std::next(child_it)).data)->data_vector.size() > MIN_LOAD_FACTOR) {
            sibling_it = std::next(child_it);
        }
        else if (child_it != node->data_vector.begin()) {
            sibling_it = std::prev(child_it);
        }
        else if (child_it != std::prev(node->data_vector.end())) {
                sibling_it = std::next(child_it);
        }

        if (sibling_it == child_it) {
            if (node != m_root) // Only root can have < MIN_LOAD_FACTOR elements, and no siblings
                throw StorageException("Something goes completely wrong");

            std::swap(*node, *child); // Move child up
            return REMOVED;
        }

        auto sibling = getPtr((*sibling_it).data);
//        auto sibling_it = (sibling == child->left_sibling ? std::prev(child_it) : std::next(child_it));
        // Merge siblings and update data
        if (sibling->data_vector.size() > MIN_LOAD_FACTOR) {
            if ((*child_it).key <= (*sibling_it).key) { // Merge with right sibling
                child->data_vector.push_back(sibling->data_vector.front());
                sibling->data_vector.erase(sibling->data_vector.begin());
            }
            else {
                child->data_vector.insert(child->data_vector.begin(), sibling->data_vector.back());
                sibling->data_vector.erase(std::prev(sibling->data_vector.end()));
            }
            (*child_it).key = child->data_vector.back().key;
            (*sibling_it).key = sibling->data_vector.back().key;
            return OK;
        }
        else { // Sibling hasn't enough nodes too. Merge them.
            if ((*child_it).key <= (*sibling_it).key) {
                std::copy(sibling->data_vector.begin(), sibling->data_vector.end(),
                          std::back_inserter(child->data_vector));
            }
            else {
                std::copy(child->data_vector.begin(), child->data_vector.end(),
                          std::back_inserter(sibling->data_vector));
                std::swap(child->data_vector, sibling->data_vector);
            }

            if (sibling == child->left_sibling) {
                child->left_sibling = sibling->left_sibling;
                if (child->left_sibling)
                    child->left_sibling->right_sibling = child;
            }
            else {
                child->right_sibling = sibling->right_sibling;
                if (child->right_sibling)
                    child->right_sibling->left_sibling = child;
            }
            (*child_it).key = child->data_vector.back().key;
            node->data_vector.erase(sibling_it);
            return REMOVED;
        }
    }

    throw StorageException((std::string(__func__) + ":  something goes completely wrong (" + std::to_string(key) + ")").c_str());
}


void BPTree::assertInvariants(BTreeNodePtr node)
{
    if (node != m_root) {
        assert(node->data_vector.size() >= MIN_LOAD_FACTOR && node->data_vector.size() <= MAX_LOAD_FACTOR);
    }
    else {
        assert(node->data_vector.size() >= 0 && node->data_vector.size() <= MAX_LOAD_FACTOR);
        if (!node->data_vector.empty()) {
            auto child = getChildWithKey(m_root, 0);
            assert(!child->left_sibling);
            child = getChildWithKey(m_root, m_root->data_vector.back().key);

            // Move to most right child (for case with same keys)
            while (child->right_sibling)
                child = child->right_sibling;
            assert(child->data_vector.back().key == m_root->data_vector.back().key);
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


DataRecord BPTree::getData(const std::any &var) const
{
    return std::any_cast<DataRecord>(var);
}


BTreeNodePtr BPTree::getPtr(const std::any &var) const
{
    return std::any_cast<BTreeNodePtr>(var);
}


void BPTree::load()
{
    if (m_blockManager->isRootInitialized()) {
        uint8_t *root_ptr = m_blockManager->getRootBlock();
        m_root = makeNodeFromData(root_ptr, 0);
    }
    else {
        m_root = std::make_shared<BTreeNode>();
        m_root->offset = 0;
        m_root->is_leaf = true;
    }
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
            uint8_t *ptr = node_ptr + DATA_OFFSET + (i * DATA_ITEM_SIZE);
            uint64_t key = *(reinterpret_cast<uint64_t*>(ptr));
            ptr += KEY_SIZE;
            uint64_t data = *(reinterpret_cast<uint64_t*>(ptr));
            ptr += DATA_SIZE;
            uint64_t path_offset = *(reinterpret_cast<uint64_t*>(ptr));
            ptr += PATH_OFFSET_SIZE;
            uint8_t is_data = *ptr;

            node->data_vector.emplace_back(
                    DataItem(key, DataRecord{.data = data, .path_offset = path_offset, .is_data = is_data})
            );
        }
    }
    else {
        for (auto i = 0; i < data_count; i++) {
            uint8_t *ptr = node_ptr + DATA_OFFSET + (i * DATA_ITEM_SIZE);
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
    if (node->offset != BTreeNode::NULL_OFFSET) {
        node_ptr = m_blockManager->getTreeNodeBlock(node->offset);
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
            uint8_t *ptr = node_ptr + DATA_OFFSET + (i * DATA_ITEM_SIZE);

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

            uint8_t *ptr = node_ptr + DATA_OFFSET + (i * DATA_ITEM_SIZE);

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


void BPTree::addData(uint64_t key, uint64_t path_off, uint64_t data, bool is_data)
{
    DataRecord real_data{.data = data, .path_offset = path_off, .is_data = is_data};
    bool need_check = internalAddData(m_root, key, real_data);
    if (need_check && m_root->data_vector.size() > MAX_LOAD_FACTOR) {
        // Need split root. Move root to new place & add fill it by new pointers
        auto new_node = std::make_shared<BTreeNode>();
        std::swap(*m_root, *new_node);
        m_root->is_leaf = false;
        m_root->offset = 0;
        new_node->offset = BTreeNode::NULL_OFFSET;
        auto sibling = makeSibling(new_node);

        auto max_left_key = new_node->data_vector.back().key;
        auto max_new_key = sibling->data_vector.back().key;
        m_root->data_vector.emplace_back(DataItem(max_left_key, new_node));
        m_root->data_vector.emplace_back(DataItem(max_new_key, sibling));
        new_node->right_sibling = sibling;
        sibling->left_sibling = new_node;
    }
}
