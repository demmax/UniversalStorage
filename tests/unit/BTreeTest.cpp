//
// Created by maxon on 27.05.18.
//

#include <gtest/gtest.h>
#include <exceptions.h>
#include "BPTree.h"
#include "utils.hpp"
#include "../mocks/StubBlockManager.h"



using namespace UniversalStorage;
//using ::testing::Return;
//using ::testing::ReturnArg;
//using ::testing::_;


//TEST(BTreeTest, AddingTest)
//{
//    BPTree tree(std::make_shared<StubBlockManager>());
//    int key = 0;
//    int val = 1;
//
//    const int N = 10000;
//    for (int i = 0; i < N; i++) {
//        tree.setValue(std::to_string(i), {0, 1, 2, 3});
//        ++key; ++val;
//    }
//
//    struct Func
//    {
//        int counter = 0;
//        int old_key = -1;
//        int old_val = 0;
//        void operator()(uint64_t key, data_type val) {
//            EXPECT_EQ(key, old_key + 1);
//            EXPECT_EQ(val.data, old_val + 1);
//            EXPECT_EQ(val.data, key + 1);
//            old_key = key;
//            old_val = val.data;
//            ++counter;
//        }
//    };
//
//    Func f;
//    tree.traverse(f);
//    EXPECT_EQ(f.counter, N);
//}


TEST(BTreeTest, GettingValuesTest)
{
    auto blockManager = std::make_shared<StubBlockManager>();
    BPTree tree(blockManager);

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
    BPTree tree(blockManager);

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
    BPTree tree(blockManager);

    const int N = 10000;
    for (uint64_t i = 0; i < N; i++) {
        tree.setValue(std::to_string(i), unpackValue(i));
    }

    tree.removeKey(std::to_string(N / 2));

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
    BPTree tree(blockManager);
    int key = 0;
    int val = 1;

    const int N = 10000;
    for (uint64_t i = 0; i < N; i++) {
        tree.setValue(std::to_string(i), unpackValue(i));
        ++key; ++val;
    }

    for (uint64_t i = 0; i < N - 1; i++) {
        tree.removeKey(std::to_string(i));
    }

    for (int i = 0; i < N - 1; i++) {
        EXPECT_THROW(tree.getValue(std::to_string(i)), NoSuchPathException);
    }
}


TEST(BTreeTest, MultiKeyTest)
{
    auto blockManager = std::make_shared<StubBlockManager>();
    BPTree tree(blockManager);
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
    BPTree tree(blockManager);
    const int N = 10000;
    for (int i = 0; i < N; i++) {
        tree.setValue(std::to_string(i), unpackValue(i));
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


    uint64_t off = N / 2;
    auto off_str = std::to_string(off);
    auto result = tree.getValue(off_str);
    EXPECT_EQ(result, unpackValue(off));

    tree.removeKey(off_str);
    EXPECT_THROW(tree.getValue(off_str), NoSuchPathException);
}

TEST(BTreeTest, SimpleStoreLoadTest)
{

}