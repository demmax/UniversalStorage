//
// Created by maxon on 26.05.18.
//

#ifndef UNIVERSALSTORAGE_BPTREE_H
#define UNIVERSALSTORAGE_BPTREE_H

#include <memory>
#include <variant>
#include <vector>

#include "IBlockManager.h"
#include <memory>
#include <any>


namespace UniversalStorage {


struct RealData
{
    uint64_t data;
    uint64_t path_offset; //!< offset to path
    bool is_data; //!< Data or pointer to data
};

using data_type = RealData;

struct BTreeNode;
struct DataItem
{
    uint64_t key;
    std::any data;
    DataItem(uint64_t k, std::any v) : key(k), data(std::move(v)) {}
    bool operator<(const DataItem &o) const { return key < o.key; }
    bool operator<(uint64_t k) const { return key < k; }
};

using BTreeNodePtr = std::shared_ptr<BTreeNode>;
struct BTreeNode
{
    uint64_t offset{ 0 };
    bool is_leaf { true };
    BTreeNodePtr left_sibling{ nullptr };
    BTreeNodePtr right_sibling{ nullptr };
    std::vector<DataItem> data_vector;
};



class BPTree
{
public:
    BPTree(IBlockManagerPtr blockManager);
    ~BPTree();

    void addKey(uint64_t key, uint64_t data_off, uint64_t path_off, bool is_data);
    std::vector<data_type> getValues(uint64_t key) const;
    void removeKey(uint64_t key, uint64_t path_offset);

    void load();
    void store();

    template <typename T>
    void traverse(T &&func)
    {
        visitNode(m_root.get(), func);
    }


protected:
    enum RemoveStatus { REMOVE_OK, NEED_SIBLING, REMOVE_MERGED, CANT_MERGE };

    BTreeNodePtr internalAddData(BTreeNodePtr node, uint64_t key, data_type data);
    RemoveStatus internalRemoveData(BTreeNodePtr node, uint64_t key, uint64_t path_offset);
    BTreeNodePtr makeSibling(BTreeNodePtr node, uint64_t key, const std::any &data);
    RemoveStatus mergeWithSibling(BTreeNodePtr node);
    BTreeNodePtr getChildWithKey(BTreeNodePtr node, uint64_t key) const;
    void insertData(BTreeNodePtr node, uint64_t key, const std::any &data);
    void clearNode(BTreeNodePtr node);
    void storeSubtree(BTreeNodePtr node);

    BTreeNodePtr getPtr(const std::any &var) const;
    data_type getData(const std::any &var) const;

    BTreeNodePtr makeNodeFromData(uint8_t *base, uint64_t offset);

    void assertInvariants(BTreeNodePtr node);

    template <typename T>
    void visitNode(BTreeNode *node, T &&func)
    {
        for (auto it = node->data_vector.begin(); it != node->data_vector.end(); ++it) {
            if (!node->is_leaf) {
                visitNode(getPtr((*it).data).get(), func);
            }
            else {
                func((*it).key, getData((*it).data));
            }
        }
    }

    BTreeNodePtr m_root;
    IBlockManagerPtr m_blockManager;

    static constexpr uint8_t MAX_LOAD_FACTOR = 100;
    static constexpr uint8_t MIN_LOAD_FACTOR = MAX_LOAD_FACTOR / 2;

    static constexpr uint64_t IS_LEAF_OFFSET = 0;
    static constexpr uint64_t LEFT_SIBLING_OFFSET = 1;
    static constexpr uint64_t RIGHT_SIBLING_OFFSET = 7;
    static constexpr uint64_t DATA_COUNT_OFFSET = 13;
    static constexpr uint64_t DATA_OFFSET = 14;
    static constexpr uint8_t  MAX_CHILD_COUNT = 100;
    static constexpr uint8_t  KEY_SIZE = 8;
    static constexpr uint8_t  DATA_SIZE = 8;
    static constexpr uint8_t  PATH_OFFSET_SIZE = 8;
    static constexpr uint8_t  IS_DATA_FLAG_SIZE = 1;
    static constexpr uint8_t  DATA_ITEM_SIZE = KEY_SIZE + DATA_SIZE + PATH_OFFSET_SIZE + IS_DATA_FLAG_SIZE;
    static constexpr uint64_t MAX_NODE_SIZE = DATA_OFFSET + (MAX_CHILD_COUNT * DATA_ITEM_SIZE);
};

}

#endif //UNIVERSALSTORAGE_BPTREE_H

