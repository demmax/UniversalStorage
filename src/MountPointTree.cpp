//
// Created by maxon on 02.05.18.
//

#include <algorithm>
#include <stack>
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

    std::unique_lock<std::shared_mutex> lock(m_treeMtx);
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
    std::shared_lock<std::shared_mutex> lock(m_treeMtx);
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

void MountPointTree::removeMountPoint(const std::string &path)
{
    std::unique_lock<std::shared_mutex> lock(m_treeMtx);
    PathView view(path);
    view.begin();
    removeStorage(m_root.get(), view);
}

bool MountPointTree::removeStorage(MountPointTreeNode *node, PathView &view)
{
    auto path_part = view.next();
    if (path_part != view.end()) {
        auto child = std::find_if(node->children.begin(), node->children.end(), [&](const std::shared_ptr<MountPointTreeNode> &node) {
            return node->rel_path == path_part;
        });

        if (child == node->children.end())
            throw NoSuchPathException("No such path");
        else {
            bool can_remove_node = removeStorage((*child).get(), view);
            if (can_remove_node)
                node->children.erase(child);
        }
    }
    else {
        node->storage_vector.clear();
    }

    return node->storage_vector.empty() && node->children.empty();
}

std::optional<MountPoint> MountPointTree::getPriorityStorage(const std::string &path) const
{
    PathView view(path);
    std::string_view path_part = view.begin();

    std::optional<MountPoint> result;

    std::optional<std::vector<MountPoint>::iterator> result_iterator; // There is no valid() method on iterator, so..
    MountPointTreeNode* current = m_root.get();
    std::shared_lock<std::shared_mutex> lock(m_treeMtx);

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

