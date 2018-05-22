//
// Created by maxon on 02.05.18.
//

#ifndef UNIVERSALSTORAGE_MOUNTPOINTTREE_H
#define UNIVERSALSTORAGE_MOUNTPOINTTREE_H

#include <string>
#include <set>
#include <shared_mutex>

#include "IStorage.h"
#include "MountPoint.h"
#include "PathView.h"

namespace UniversalStorage {


class MountPointTree
{
public:
    MountPointTree();
    void addMountPoint(const std::string &path, IStoragePtr storage, const std::string &phys_path, size_t priority);
    void removeMountPoint(const std::string &path);
    std::optional<MountPoint> getPriorityStorage(const std::string &path) const;
    std::set<MountPoint> getSuitableStorageList(const std::string &path) const;

protected:
    struct MountPointTreeNode
    {
        std::string rel_path;
        std::vector<MountPoint> storage_vector;
        std::vector<std::shared_ptr<MountPointTreeNode>> children;

        MountPointTreeNode(std::string path) : rel_path(std::move(path)) {}
    };

    MountPointTreeNode *make_node(MountPointTreeNode *current, std::string_view path);
    bool removeStorage(MountPointTreeNode *node, PathView &view);

    std::shared_ptr<MountPointTreeNode> m_root;
    mutable std::shared_mutex m_treeMtx;
};

}

#endif //UNIVERSALSTORAGE_MOUNTPOINTTREE_H
