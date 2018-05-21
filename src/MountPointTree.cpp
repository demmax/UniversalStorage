//
// Created by maxon on 02.05.18.
//

#include <algorithm>
#include "MountPointTree.h"
#include "exceptions.h"
#include "PathView.h"


using namespace UniversalStorage;


MountPointTree::MountPointTree()
{
    m_root = std::make_shared<MountPointTreeNode>("/");
}


void MountPointTree::addMountPoint(const std::string &mount_path, IStoragePtr storage, const std::string &phys_path, size_t priority)
{
    PathView view(mount_path);
    MountPointTreeNode* current = m_root.get();
    std::string_view path_part = view.begin();

    for (path_part = view.next(); path_part != view.end(); path_part = view.next())
    {
        auto it = std::find_if(current->children.begin(), current->children.end(),
                [&](const std::shared_ptr<MountPointTreeNode> &node) {
            return node->rel_path == path_part;
        });

        if (it == current->children.end())
            current = make_node(current, path_part);
        else {
            current = (*it).get();
        }
    }

    current->storage_vector.emplace_back(storage, phys_path, mount_path, priority);
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
        auto it = std::find_if(current->children.begin(), current->children.end(), [&](const std::shared_ptr<MountPointTreeNode> &node) {
            return node->rel_path == path_part;
        });

        if (it == current->children.end())
            break;
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
    removeStorage(m_root.get(), std::move(storage));
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

std::optional<MountPoint> MountPointTree::getPriorityStorage(const std::string &path) const
{
    PathView view(path);
    std::string_view path_part = view.begin();

    std::optional<MountPoint> result;

    std::optional<std::vector<MountPoint>::iterator> result_iterator; // There is no valid() method on iterator, so..
    MountPointTreeNode* current = m_root.get();
    if (!current->storage_vector.empty())
        result_iterator = std::max_element(current->storage_vector.begin(), current->storage_vector.end());

    for (path_part = view.next(); path_part != view.end(); path_part = view.next())
    {
        auto it = std::find_if(current->children.begin(), current->children.end(), [&](const std::shared_ptr<MountPointTreeNode> &node) {
            return node->rel_path == path_part;
        });

        if (it == current->children.end())
            break;
        else {
            current = (*it).get();
        }

        if (!current->storage_vector.empty()) {
            auto max_storage = std::max_element(current->storage_vector.begin(), current->storage_vector.end());
            if (!result_iterator || *result_iterator.value() < *max_storage)
                result_iterator = max_storage;
        }
    }

    if (result_iterator)
        result = *(result_iterator.value());

    return result;
}

