//
// Created by maxon on 27.05.18.
//

#include <gtest/gtest.h>
#include <exceptions.h>
#include "BPTree.h"
#include "../mocks/MockBlockManager.h"



using namespace UniversalStorage;
using ::testing::Return;
using ::testing::ReturnArg;
using ::testing::_;


TEST(BTreeTest, AddingTest)
{
    BPTree tree(std::make_shared<MockBlockManager>());
    int key = 0;
    int val = 1;

    const int N = 10000;
    for (int i = 0; i < N; i++) {
        tree.addKey(key, val, 0, true);
        ++key; ++val;
    }

    struct Func
    {
        int counter = 0;
        int old_key = -1;
        int old_val = 0;
        void operator()(uint64_t key, data_type val) {
            EXPECT_EQ(key, old_key + 1);
            EXPECT_EQ(val.data, old_val + 1);
            EXPECT_EQ(val.data, key + 1);
            old_key = key;
            old_val = val.data;
            ++counter;
        }
    };

    Func f;
    tree.traverse(f);
    EXPECT_EQ(f.counter, N);
}


TEST(BTreeTest, GettingValuesTest)
{
    auto blockManager = std::make_shared<MockBlockManager>();
    BPTree tree(blockManager);
    int key = 0;
    int val = 1;

    const int N = 10000;
    for (int i = 0; i < N; i++) {
        tree.addKey(key, val, key, true);
        ++key; ++val;
    }

    for (int i = 0; i < N; i++) {
        EXPECT_CALL(*blockManager, getPathString(_)).WillRepeatedly(Return(std::to_string(i)));
        auto val = tree.getValue(i, std::to_string(i));
        EXPECT_EQ(val.data, i + 1);
    }
}

TEST(BTreeTest, GettingValuesReversedTest)
{
    auto blockManager = std::make_shared<MockBlockManager>();
    BPTree tree(blockManager);
    int key = 10000;
    int val = 10001;

    const int N = 10000;
    for (int i = 0; i < N; i++) {
        tree.addKey(key, val, key, true);
        --key; --val;
    }

    for (int i = 1; i < N; i++) {
        EXPECT_CALL(*blockManager, getPathString(_)).WillRepeatedly(Return(std::to_string(i)));
        auto val = tree.getValue(i, std::to_string(i));
        EXPECT_EQ(val.data, i + 1);
    }
}

TEST(BTreeTest, OneElementRemovingTest)
{
    auto blockManager = std::make_shared<MockBlockManager>();
    BPTree tree(blockManager);
    int key = 0;
    int val = 1;

    const int N = 10000;
    for (uint64_t i = 0; i < N; i++) {
        tree.addKey(key, val, key, true);
        ++key; ++val;
    }

    EXPECT_CALL(*blockManager, getPathString(_)).WillRepeatedly(::testing::Invoke(
            [=](uint64_t off) {
                return std::to_string(off);
            }));

        tree.removeKey(N / 2, std::to_string(N / 2));

    for (int i = 0; i < N / 2; i++) {
        EXPECT_CALL(*blockManager, getPathString(_)).WillRepeatedly(Return(std::to_string(i)));
        auto val = tree.getValue(i, std::to_string(i));
        EXPECT_EQ(val.data, i + 1);
    }

    for (int i = N / 2 + 1; i < N / 2; i++) {
        EXPECT_CALL(*blockManager, getPathString(_)).WillRepeatedly(Return(std::to_string(i)));
        auto val = tree.getValue(i, std::to_string(i));
        EXPECT_EQ(val.data, i + 1);
    }

    EXPECT_THROW(tree.getValue(N / 2, std::to_string(N / 2)), NoSuchPathException);
}

TEST(BTreeTest, AllRemovingTest)
{
    auto blockManager = std::make_shared<MockBlockManager>();
    BPTree tree(blockManager);
    int key = 0;
    int val = 1;

    const int N = 10000;
    for (uint64_t i = 0; i < N; i++) {
        tree.addKey(key, val, key, true);
        ++key; ++val;
    }

    EXPECT_CALL(*blockManager, getPathString(_)).WillRepeatedly(::testing::Invoke(
            [=](uint64_t off) {
                return std::to_string(off);
    }));

    for (uint64_t i = 0; i < N - 1; i++) {
        tree.removeKey(i, std::to_string(i));
    }

    for (int i = 0; i < N - 1; i++) {
        EXPECT_CALL(*blockManager, getPathString(_)).WillRepeatedly(Return(std::to_string(i)));
        EXPECT_THROW(tree.getValue(i, std::to_string(i)), NoSuchPathException);
    }
}


TEST(BTreeTest, MultiKeyTest)
{
    auto blockManager = std::make_shared<MockBlockManager>();
    BPTree tree(blockManager);
    const int N = 1000;
    for (int i = 0; i < N; i++) {
        tree.addKey(1, i, i, true);
    }

    EXPECT_CALL(*blockManager, getPathString(_)).WillRepeatedly(::testing::Invoke(
            [=](uint64_t off) {
                uint64_t a = off;
                return std::to_string(a);
            }));
    auto result = tree.getValue(1, "500");
    EXPECT_EQ(result.data, 500);
    EXPECT_EQ(result.path_offset, 500);
    EXPECT_TRUE(result.is_data);
}

TEST(BTreeTest, MultiKeyRemoveTest)
{
    auto blockManager = std::make_shared<MockBlockManager>();
    BPTree tree(blockManager);
    const int N = 10000;
    for (int i = 0; i < N; i++) {
        tree.addKey(1, i, i, true);
    }

//    struct Func
//    {
//        size_t count {0};
//        void operator()(uint64_t key, data_type val) {
//            ++count;
//            if (key != 1)
//                throw "sadf";
//        }
//    };
//
//    Func f;
//    tree.traverse(f);

    EXPECT_CALL(*blockManager, getPathString(_)).WillRepeatedly(::testing::Invoke(
            [=](uint64_t off) {
                return std::to_string(off);
            }));

    uint64_t off = N / 2;
    auto off_str = std::to_string(off);
    auto result = tree.getValue(1, off_str);
    EXPECT_EQ(result.data, off);
    EXPECT_EQ(result.path_offset, off);
    EXPECT_TRUE(result.is_data);

    tree.removeKey(1, off_str);
    EXPECT_THROW(tree.getValue(1, off_str), NoSuchPathException);
}

TEST(BTreeTest, SimpleStoreLoadTest)
{

}