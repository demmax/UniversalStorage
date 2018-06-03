//
// Created by maxon on 26.05.18.
//

#ifndef UNIVERSALSTORAGE_BPTREE_H
#define UNIVERSALSTORAGE_BPTREE_H

#include <memory>
#include <variant>
#include <vector>

#include "IBlockManager.h"
#include "IStorage.h"
#include <memory>
#include <any>


namespace UniversalStorage {


struct DataRecord
{
    uint64_t data; //!< data or data offset
    uint64_t path_offset; //!< offset to path
    bool is_data; //!< is data or pointer to data
};


struct BTreeNode;
struct DataItem
{
    uint64_t key;
    std::any data; //!< DataRecords or BTreeNodePtr
    DataItem(uint64_t k, std::any v) : key(k), data(std::move(v)) {}
    bool operator<(const DataItem &o) const { return key < o.key; }
    bool operator<(uint64_t k) const { return key < k; }
};

using BTreeNodePtr = std::shared_ptr<BTreeNode>;
struct BTreeNode
{
    static constexpr uint64_t NULL_OFFSET = std::numeric_limits<uint64_t>::max();
    uint64_t offset{ NULL_OFFSET };
    bool is_leaf { true };
    BTreeNodePtr left_sibling{ nullptr };
    BTreeNodePtr right_sibling{ nullptr };
    std::vector<DataItem> data_vector;
};



class BPTreeStorage : public IStorage
{
public:
    BPTreeStorage(IBlockManagerPtr blockManager);
    ~BPTreeStorage();

    void setValue(const std::string &path, const std::vector<uint8_t> &data) override;
    std::vector<uint8_t> getValue(const std::string &path) const override;
    void removeValue(const std::string &path) override;
    bool isExist(const std::string &path) const override;

    void load();
    void store();

    template <typename T>
    void traverse(T &&func)
    {
        visitNode(m_root.get(), func);
    }


protected:
    enum RemoveStatus { OK, REMOVED, NOT_FOUND };

    // Internal tree functions
    void addData(uint64_t key, uint64_t path_off, uint64_t data, bool is_data);
    bool updateValue(uint64_t key, const std::string &path, uint64_t data, bool is_data);
    bool internalAddData(BTreeNodePtr node, uint64_t key, DataRecord data);
    RemoveStatus internalRemoveData(BTreeNodePtr node, uint64_t key, const std::string &path);
    BTreeNodePtr makeSibling(BTreeNodePtr node);
    BTreeNodePtr getChildWithKey(BTreeNodePtr node, uint64_t key) const;
    void insertData(BTreeNodePtr node, uint64_t key, const std::any &data);

    // Helpers
    BTreeNodePtr getPtr(const std::any &var) const;
    DataRecord getData(const std::any &var) const;

    // Load/storing data
    void storeSubtree(BTreeNodePtr node);
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
    static constexpr uint64_t RIGHT_SIBLING_OFFSET = 9;
    static constexpr uint64_t DATA_COUNT_OFFSET = 17;
    static constexpr uint64_t DATA_OFFSET = 18;
    static constexpr uint8_t  MAX_CHILD_COUNT = 81;
    static constexpr uint8_t  KEY_SIZE = 8;
    static constexpr uint8_t  DATA_SIZE = 8;
    static constexpr uint8_t  PATH_OFFSET_SIZE = 8;
    static constexpr uint8_t  IS_DATA_FLAG_SIZE = 1;
    static constexpr uint8_t  DATA_ITEM_SIZE = KEY_SIZE + DATA_SIZE + PATH_OFFSET_SIZE + IS_DATA_FLAG_SIZE;
    static constexpr uint64_t MAX_NODE_SIZE = DATA_OFFSET + (MAX_CHILD_COUNT * DATA_ITEM_SIZE);

    static_assert(MAX_NODE_SIZE <= IBlockManager::TREE_NODE_BLOCK_SIZE);
};

}

#endif //UNIVERSALSTORAGE_BPTREE_H

