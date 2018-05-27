//
// Created by maxon on 27.05.18.
//

#include <gtest/gtest.h>
#include <exceptions.h>
#include "BPTree.h"
#include "../mocks/MockBlockManager.h"



using namespace UniversalStorage;



TEST(BTreeTest, AddingTest)
{
    BPTree tree(std::make_shared<MockBlockManager>());
    int key = 0;
    int val = 1;

    const int N = 10000;
    for (int i = 0; i < N; i++) {
        tree.addKey(key, val, true);
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
    BPTree tree(std::make_shared<MockBlockManager>());
    int key = 0;
    int val = 1;

    const int N = 10000;
    for (int i = 0; i < N; i++) {
        tree.addKey(key, val, true);
        ++key; ++val;
    }

    for (int i = 0; i < N; i++) {
        auto val = tree.getValue(i);
        EXPECT_EQ(val.front().data, i + 1);
    }
}

TEST(BTreeTest, GettingValuesReversedTest)
{
    BPTree tree(std::make_shared<MockBlockManager>());
    int key = 10000;
    int val = 10001;

    const int N = 10000;
    for (int i = 0; i < N; i++) {
        tree.addKey(key, val, true);
        --key; --val;
    }

    for (int i = 1; i < N; i++) {
        auto val = tree.getValue(i);
        EXPECT_EQ(val.front().data, i + 1);
    }
}

TEST(BTreeTest, SimpleRemovingTest)
{
    BPTree tree(std::make_shared<MockBlockManager>());
    int key = 0;
    int val = 1;

    const int N = 10000;
    for (int i = 0; i < N; i++) {
        tree.addKey(key, val, true);
        ++key; ++val;
    }

    for (int i = 0; i < N - 1; i++) {
        tree.removeKey(i);
    }

    tree.removeKey(N-1);
    for (int i = 0; i < N; i++) {
        EXPECT_THROW(tree.getValue(i), NoSuchPathException);
    }
}


TEST(BTreeTest, MultiKeyTest)
{
    BPTree tree(std::make_shared<MockBlockManager>());
    const int N = 1000;
    for (int i = 0; i < N; i++) {
        tree.addKey(1, i, true);
    }

    auto result = tree.getValue(1);
    std::sort(result.begin(), result.end(), [](const RealData &d1, const RealData &d2) {
        return d1.data < d2.data;
    });
    EXPECT_EQ(result.size(), N);
    for (int i = 0; i < N; i++) {
        EXPECT_EQ(result[i].data, i);
    }
}