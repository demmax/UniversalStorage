//
// Created by maxon on 02.05.18.
//

#ifndef UNIVERSALSTORAGE_MOUNTPOINTTREE_H
#define UNIVERSALSTORAGE_MOUNTPOINTTREE_H

#include <string>
#include <set>

#include "IStorage.h"


struct MountPoint
{
    IStoragePtr storage;
    uint32_t priority;
    std::string path;
    MountPoint(IStoragePtr s, std::string _path, uint32_t p)
            : storage(std::move(s)), path(std::move(_path)), priority(p) {}
    bool operator<(const MountPoint &o) const { return priority > o.priority; }
};


class MountPointTree
{
public:
    MountPointTree();
    void addMountPoint(const std::string &path, IStoragePtr storage, const std::string &real_path, uint32_t priority);
    void removeMountPoint(IStoragePtr storage);
    std::set<MountPoint> getSuitableStorageList(const std::string &path) const;

protected:
    struct MountPointTreeNode
    {
        std::string rel_path;
        std::vector<MountPoint> storage_vector;
        std::vector<std::shared_ptr<MountPointTreeNode>> children;

        MountPointTreeNode(std::string path) : rel_path(std::move(path)) {}
    };

    MountPointTreeNode* make_node(MountPointTreeNode* current, std::string_view path);
    void removeStorage(MountPointTreeNode *node, IStoragePtr storage);

    std::shared_ptr<MountPointTreeNode> m_root;
};


#endif //UNIVERSALSTORAGE_MOUNTPOINTTREE_H
