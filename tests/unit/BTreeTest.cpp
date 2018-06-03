//
// Created by maxon on 27.05.18.
//

#include <gtest/gtest.h>
#include <exceptions.h>
#include "BPTreeStorage.h"
#include "utils.hpp"
#include "../mocks/StubBlockManager.h"



using namespace UniversalStorage;
//using ::testing::Return;
//using ::testing::ReturnArg;
//using ::testing::_;



TEST(BTreeTest, GettingValuesTest)
{
    auto blockManager = std::make_shared<StubBlockManager>();
    BPTreeStorage tree(blockManager);

    const int N = 10000;
    for (int i = 0; i < N; i++) {
        tree.setValue(std::to_string(i), unpackValue(i));
    }

    for (int i = 0; i < N; i++) {
        auto val = tree.getValue(std::to_string(i));
        EXPECT_EQ(val, unpackValue(i));
    }
}

TEST(BTreeTest, GettingValuesReversedTest)
{
    auto blockManager = std::make_shared<StubBlockManager>();
    BPTreeStorage tree(blockManager);

    const int N = 10000;
    for (int i = 0; i < N; i++) {
        tree.setValue(std::to_string(i), unpackValue(i));
    }

    for (int i = 1; i < N; i++) {
        auto val = tree.getValue(std::to_string(i));
        EXPECT_EQ(val, unpackValue(i));
    }
}

TEST(BTreeTest, OneElementRemovingTest)
{
    auto blockManager = std::make_shared<StubBlockManager>();
    BPTreeStorage tree(blockManager);

    const int N = 10000;
    for (uint64_t i = 0; i < N; i++) {
        tree.setValue(std::to_string(i), unpackValue(i));
    }

    tree.removeValue(std::to_string(N / 2));

    for (int i = 0; i < N / 2; i++) {
        auto val = tree.getValue(std::to_string(i));
        EXPECT_EQ(val, unpackValue(i));
    }

    for (int i = N / 2 + 1; i < N / 2; i++) {
        auto val = tree.getValue(std::to_string(i));
        EXPECT_EQ(val, unpackValue(i));
    }

    EXPECT_THROW(tree.getValue(std::to_string(N / 2)), NoSuchPathException);
}

TEST(BTreeTest, AllRemovingTest)
{
    auto blockManager = std::make_shared<StubBlockManager>();
    BPTreeStorage tree(blockManager);
    int key = 0;
    int val = 1;

    const int N = 10000;
    for (uint64_t i = 0; i < N; i++) {
        tree.setValue(std::to_string(i), unpackValue(i));
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
    auto blockManager = std::make_shared<StubBlockManager>();
    BPTreeStorage tree(blockManager);
    const int N = 1000;
    for (int i = 0; i < N; i++) {
        tree.setValue(std::to_string(i), unpackValue(i));
    }

    auto result = tree.getValue("500");
    EXPECT_EQ(result, unpackValue(500));
}

TEST(BTreeTest, MultiKeyRemoveTest)
{
    auto blockManager = std::make_shared<StubBlockManager>();
    BPTreeStorage tree(blockManager);
    const int N = 10000;
    for (int i = 0; i < N; i++) {
        tree.setValue(std::to_string(i), unpackValue(i));
    }

    uint64_t off = N / 2;
    auto off_str = std::to_string(off);
    auto result = tree.getValue(off_str);
    EXPECT_EQ(result, unpackValue(off));

    tree.removeValue(off_str);
    EXPECT_THROW(tree.getValue(off_str), NoSuchPathException);
}

TEST(BTreeTest, RootStoreLoadTest)
{
    auto blockManager = std::make_shared<StubBlockManager>();
    {
        BPTreeStorage tree(blockManager);
        tree.setValue("/", unpackValue(0xDEADBEAF));
    }

    {
        BPTreeStorage tree(blockManager);
        auto val = tree.getValue("/");
        EXPECT_EQ(val, unpackValue(0xDEADBEAF));
    }
}


TEST(BTreeTest, SimpleStoreLoadTest)
{
    auto blockManager = std::make_shared<StubBlockManager>();
    const int N = 10;

    {
        BPTreeStorage tree(blockManager);
        for (int i = 0; i < N; i++) {
            tree.setValue(std::to_string(i), unpackValue(i));
        }
    }

    {
        BPTreeStorage tree(blockManager);
        for (int i = 0; i < N; i++) {
            auto val = tree.getValue(std::to_string(i));
            EXPECT_EQ(val, unpackValue(i));
        }
    }
}