//
// Created by maxon on 02.05.18.
//

#include <algorithm>
#include "MountPointTree.h"
#include "exceptions.h"
#include "PathView.h"


MountPointTree::MountPointTree()
{
    m_root = std::make_shared<MountPointTreeNode>("/");
}


void MountPointTree::addMountPoint(const std::string &path, IStoragePtr storage, uint32_t priority)
{
    PathView view(path);
    MountPointTreeNode* current = m_root.get();
    std::string_view path_part = view.begin();

    for (path_part = view.next(); path_part != view.end(); path_part = view.next())
    {
        auto it = std::find_if(current->children.begin(), current->children.end(),
                [&](std::shared_ptr<MountPointTreeNode> node) {
            return node->rel_path == path_part;
        });

        if (it == current->children.end())
            current = make_node(current, path_part);
        else {
            current = (*it).get();
        }
    }

    current->storage_vector.emplace_back(storage, priority);
}

std::set<MountPoint> MountPointTree::getSuitableStorageList(const std::string &path) const
{
    PathView view(path);
    std::string_view path_part = view.begin();

    std::set<MountPoint> result;

    MountPointTreeNode* current = m_root.get();
    if (!current->storage_vector.empty())
        result.insert(current->storage_vector.begin(), current->storage_vector.end());

    for (path_part = view.next(); path_part != view.end(); path_part = view.next())
    {
        auto it = std::find_if(current->children.begin(), current->children.end(), [&](std::shared_ptr<MountPointTreeNode> node) {
            return node->rel_path == path_part;
        });

        if (it == current->children.end())
            throw NoSuchPathException(path.c_str());
        else {
            current = (*it).get();
        }

        result.insert(current->storage_vector.begin(), current->storage_vector.end());
    }

    return result;
}

MountPointTree::MountPointTreeNode* MountPointTree::make_node(MountPointTreeNode* current, std::string_view path)
{
    current->children.emplace_back(std::make_shared<MountPointTreeNode>(std::string(path)));
    return current->children.back().get();
}

void MountPointTree::removeMountPoint(IStoragePtr storage)
{
    removeStorage(m_root.get(), storage);
}

void MountPointTree::removeStorage(MountPointTreeNode *node, IStoragePtr storage)
{
    node->storage_vector.erase(
            std::remove_if(node->storage_vector.begin(), node->storage_vector.end(), [&](const MountPoint &point) -> bool {
                return point.storage == storage;
            }), node->storage_vector.end()
    );

    for (const auto &child : node->children) {
        removeStorage(child.get(), storage);
    }
}

