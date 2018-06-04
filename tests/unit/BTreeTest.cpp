//
// Created by maxon on 27.05.18.
//

#include <gtest/gtest.h>
#include <exceptions.h>
#include "BPTreeStorage.h"
#include "../mocks/StubBlockManager.h"
#include "MappedFileBlockManager.h"
#include "utils.h"



using namespace UniversalStorage;
//using ::testing::Return;
//using ::testing::ReturnArg;
//using ::testing::_;



const std::string fileName = "data";
struct MappedManager
{
    static constexpr const char* fileName = "data";
    std::shared_ptr<MappedFileBlockManager> blockManager;
    MappedManager() {
        blockManager = std::make_shared<MappedFileBlockManager>(fileName);
    }

    ~MappedManager() {
        std::remove(fileName);
    }
};

TEST(BTreeTest, GettingValuesTest)
{
//    auto blockManager = std::make_shared<StubBlockManager>();
    MappedManager manager;
    auto blockManager = manager.blockManager;
    BPTreeStorage tree(blockManager);

    const int N = 10000;
    for (int i = 0; i < N; i++) {
        tree.setValue(std::to_string(i), Utils::unpackValue(i));
    }

    for (int i = 0; i < N; i++) {
        auto val = tree.getValue(std::to_string(i));
        EXPECT_EQ(val, Utils::unpackValue(i));
    }
}

TEST(BTreeTest, GettingValuesReversedTest)
{
    MappedManager manager;
    auto blockManager = manager.blockManager;
//    auto blockManager = std::make_shared<StubBlockManager>();
    BPTreeStorage tree(blockManager);

    const int N = 10000;
    for (int i = 0; i < N; i++) {
        tree.setValue(std::to_string(i), Utils::unpackValue(i));
    }

    for (int i = 1; i < N; i++) {
        auto val = tree.getValue(std::to_string(i));
        EXPECT_EQ(val, Utils::unpackValue(i));
    }
}

TEST(BTreeTest, OneElementRemovingTest)
{
    MappedManager manager;
    auto blockManager = manager.blockManager;
    BPTreeStorage tree(blockManager);

    const int N = 10000;
    for (uint64_t i = 0; i < N; i++) {
        tree.setValue(std::to_string(i), Utils::unpackValue(i));
    }

    tree.removeValue(std::to_string(N / 2));

    for (int i = 0; i < N / 2; i++) {
        auto val = tree.getValue(std::to_string(i));
        EXPECT_EQ(val, Utils::unpackValue(i));
    }

    for (int i = N / 2 + 1; i < N / 2; i++) {
        auto val = tree.getValue(std::to_string(i));
        EXPECT_EQ(val, Utils::unpackValue(i));
    }

    EXPECT_THROW(tree.getValue(std::to_string(N / 2)), NoSuchPathException);
}

TEST(BTreeTest, AllRemovingTest)
{
    MappedManager manager;
    auto blockManager = manager.blockManager;

//    auto blockManager = std::make_shared<StubBlockManager>();
    BPTreeStorage tree(blockManager);
    int key = 0;
    int val = 1;

    const int N = 10000;
    for (uint64_t i = 0; i < N; i++) {
        tree.setValue(std::to_string(i), Utils::unpackValue(i));
        ++key; ++val;
    }

    for (uint64_t i = 0; i < N - 1; i++) {
        tree.removeValue(std::to_string(i));
    }

    for (int i = 0; i < N - 1; i++) {
        EXPECT_THROW(tree.getValue(std::to_string(i)), NoSuchPathException);
    }
}


TEST(BTreeTest, MultiKeyTest)
{
    MappedManager manager;
    auto blockManager = manager.blockManager;
//    auto blockManager = std::make_shared<StubBlockManager>();
    BPTreeStorage tree(blockManager);
    const int N = 1000;
    for (int i = 0; i < N; i++) {
        tree.setValue(std::to_string(i), Utils::unpackValue(i));
    }

    auto result = tree.getValue("500");
    EXPECT_EQ(result, Utils::unpackValue(500));
}

TEST(BTreeTest, MultiKeyRemoveTest)
{
//    auto blockManager = std::make_shared<StubBlockManager>();
    MappedManager manager;
    auto blockManager = manager.blockManager;
    BPTreeStorage tree(blockManager);
    const int N = 10000;
    for (int i = 0; i < N; i++) {
        tree.setValue(std::to_string(i), Utils::unpackValue(i));
    }

    uint64_t off = N / 2;
    auto off_str = std::to_string(off);
    auto result = tree.getValue(off_str);
    EXPECT_EQ(result, Utils::unpackValue(off));

    tree.removeValue(off_str);
    EXPECT_THROW(tree.getValue(off_str), NoSuchPathException);
}

TEST(BTreeTest, RootStoreLoadTest)
{
    MappedManager manager;
    auto blockManager = manager.blockManager;
//    auto blockManager = std::make_shared<StubBlockManager>();
    {
        BPTreeStorage tree(blockManager);
        tree.setValue("/", Utils::unpackValue(0xDEADBEAF));
    }

    {
        BPTreeStorage tree(blockManager);
        auto val = tree.getValue("/");
        EXPECT_EQ(val, Utils::unpackValue(0xDEADBEAF));
    }
}


TEST(BTreeTest, SimpleStoreLoadTest)
{
    MappedManager manager;
    auto blockManager = manager.blockManager;
//    auto blockManager = std::make_shared<StubBlockManager>();
    const int N = 10;

    {
        BPTreeStorage tree(blockManager);
        for (int i = 0; i < N; i++) {
            tree.setValue(std::to_string(i), Utils::unpackValue(i));
        }
    }

    {
        BPTreeStorage tree(blockManager);
        for (int i = 0; i < N; i++) {
            auto val = tree.getValue(std::to_string(i));
            EXPECT_EQ(val, Utils::unpackValue(i));
        }
    }
}