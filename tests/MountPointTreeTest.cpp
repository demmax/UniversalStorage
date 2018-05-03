//
// Created by maxon on 02.05.18.
//

#include <gtest/gtest.h>
#include "MountPointTree.h"
#include "mocks/MockPhysicalStorage.h"

TEST(MountPointTreeRootCase, MountPointTreeTest)
{
    MountPointTree tree;
    std::shared_ptr storage = std::make_shared<MockPhysicalStorage>();
    uint32_t priority = 42;
    tree.addMountPoint("/", storage, "/", priority);

    auto storages = tree.getSuitableStorageList("/");
    EXPECT_EQ(storages.size(), 1);
    auto point = *(storages.begin());
    EXPECT_EQ(point.storage, storage);
    EXPECT_EQ(point.priority, priority);
}


TEST(MountPointTreeNotRootCase, MountPointTreeTest)
{
    MountPointTree tree;
    std::shared_ptr storage = std::make_shared<MockPhysicalStorage>();
    uint32_t priority = 42;
    std::string path = "/a/b/c";
    tree.addMountPoint(path, storage, "/", priority);

    auto storages = tree.getSuitableStorageList(path);
    EXPECT_EQ(storages.size(), 1);
    auto point = *(storages.begin());
    EXPECT_EQ(point.storage, storage);
    EXPECT_EQ(point.priority, priority);
}

TEST(MountPointTreeSuitableTwoOfThreeCase, MountPointTreeTest)
{
    MountPointTree tree;
    std::shared_ptr storage1 = std::make_shared<MockPhysicalStorage>();
    std::shared_ptr storage2 = std::make_shared<MockPhysicalStorage>();
    std::shared_ptr storage3 = std::make_shared<MockPhysicalStorage>();

    uint32_t priority1 = 12;
    uint32_t priority2 = 42;
    uint32_t priority3 = 43;

    std::string path1 = "/a/b";
    std::string path2 = "/a/b/d/h";
    std::string path3 = "/a/g/c";

    tree.addMountPoint(path1, storage1, "/", priority1);
    tree.addMountPoint(path2, storage2, "/", priority2);
    tree.addMountPoint(path3, storage3, "/", priority3);

    auto storages = tree.getSuitableStorageList(path2);
    EXPECT_EQ(storages.size(), 2);
    auto it = storages.begin();
    auto first = *it;
    auto second = *++it;
    EXPECT_EQ(first.storage, storage2);
    EXPECT_EQ(first.priority, priority2);
    EXPECT_EQ(second.storage, storage1);
    EXPECT_EQ(second.priority, priority1);
}


TEST(MountPointTreeRemovingStorageCase, MountPointTreeTest)
{
    MountPointTree tree;
    std::shared_ptr storage1 = std::make_shared<MockPhysicalStorage>();
    std::shared_ptr storage2 = std::make_shared<MockPhysicalStorage>();
    std::shared_ptr storage3 = std::make_shared<MockPhysicalStorage>();
    std::shared_ptr storage4 = std::make_shared<MockPhysicalStorage>();

    uint32_t priority1 = 12;
    uint32_t priority2 = 42;
    uint32_t priority3 = 43;
    uint32_t priority4 = 33;

    std::string path1 = "/a/b";
    std::string path2 = "/a/b/d/h";
    std::string path3 = "/a/g/c";
    std::string path4 = "/a/b/d";

    tree.addMountPoint(path1, storage1, "/", priority1);
    tree.addMountPoint(path2, storage2, "/", priority2);
    tree.addMountPoint(path3, storage3, "/", priority3);
    tree.addMountPoint(path4, storage4, "/", priority4);

    tree.removeMountPoint(storage2);

    auto storages = tree.getSuitableStorageList(path2);
    EXPECT_EQ(storages.size(), 2);
    auto it = storages.begin();
    auto first = *it;
    auto second = *++it;
    EXPECT_EQ(first.storage, storage4);
    EXPECT_EQ(first.priority, priority4);
    EXPECT_EQ(second.storage, storage1);
    EXPECT_EQ(second.priority, priority1);
}


TEST(MountPointOneStorageFewPlacesCase, MountPointTreeTest)
{
    MountPointTree tree;
    std::shared_ptr storage1 = std::make_shared<MockPhysicalStorage>();
    std::shared_ptr storage2 = std::make_shared<MockPhysicalStorage>();
    std::shared_ptr storage3 = std::make_shared<MockPhysicalStorage>();

    uint32_t priority1 = 12;
    uint32_t priority2 = 42;
    uint32_t priority3 = 43;
    uint32_t priority4 = 46;

    std::string path1 = "/a/b";
    std::string path2 = "/a/b/d/h";
    std::string path3 = "/a/g/c";
    std::string path4 = "/a";

    tree.addMountPoint(path1, storage1, "/", priority1);
    tree.addMountPoint(path2, storage2, "/", priority2);
    tree.addMountPoint(path3, storage3, "/", priority3);
    tree.addMountPoint(path4, storage2, "/", priority4);

    tree.removeMountPoint(storage2);

    auto storages = tree.getSuitableStorageList(path2);
    EXPECT_EQ(storages.size(), 1);
    auto point = *(storages.begin());
    EXPECT_EQ(point.storage, storage1);
    EXPECT_EQ(point.priority, priority1);
}

