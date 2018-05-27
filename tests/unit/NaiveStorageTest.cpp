//
// Created by maxon on 04.05.18.
//

#include <gtest/gtest.h>
#include <exceptions.h>
#include "NaiveStorage.h"

using namespace UniversalStorage;

TEST(NaiveStorageTest, SimpleSetGetCase)
{
    size_t count = 5000;
    std::string file_name = "file";
    std::string path = "/";
    std::vector<uint8_t> vec{1, 2, 3, 4};
    {
        NaiveStorage storage(file_name);
        for (int i = 0; i < count; i++) {
            std::vector v = vec;
            v.push_back(i);
            storage.setValue(path + std::to_string(i), v);
        }
    }

    {
        NaiveStorage storage(file_name);
        for (size_t i = 0; i < count; i++) {
            auto expected_vec = vec;
            expected_vec.push_back(i % 256);
            EXPECT_EQ(storage.getValue(path + std::to_string(i)), expected_vec);
        }
    }
    std::remove(file_name.c_str());
}


TEST(NaiveStorageTest, ConnectingToStorageSetGetCase)
{
    std::string file_name = "file1";
    size_t count = 5000;
    std::string path = "/";
    std::vector<uint8_t> vec{1, 2, 3, 4};

    {
        NaiveStorage storage(file_name);
        for (auto i = 0u; i < count; i++) {
            std::vector v = vec;
            v.push_back(i);
            storage.setValue(path + std::to_string(i), v);
        }
    }

    {
        NaiveStorage storage(file_name);
        for (size_t i = 0u; i < count; i++) {
            auto expected_vec = vec;
            expected_vec.push_back(i % 256);
            EXPECT_EQ(storage.getValue(path + std::to_string(i)), expected_vec);
        }
    }

    std::remove(file_name.c_str());
}


TEST(NaiveStorageTest, SimpleRemoveItemStorageTest)
{
    std::string file_name = "file";
    std::string path = "/";
    std::vector<uint8_t> vec{1, 2, 3, 4};

    {
        NaiveStorage storage(file_name);
        storage.setValue(path, vec);
    }
    {
        NaiveStorage storage(file_name);
        storage.removeValue(path);
        EXPECT_THROW(storage.getValue(path), NoSuchPathException);
    }
    std::remove(file_name.c_str());
}


TEST(NaiveStorageTest, RemoveExactlyOneItemStorageTest)
{
    std::string file_name = "file";
    std::string path1 = "/";
    std::string path2 = "/a";
    std::vector<uint8_t> vec1{1, 2, 3, 4};
    std::vector<uint8_t> vec2{5, 6, 7};

    {
        NaiveStorage storage(file_name);
        storage.setValue(path1, vec1);
        storage.setValue(path2, vec2);
    }

    {
        NaiveStorage storage(file_name);
        storage.removeValue(path1);
        EXPECT_EQ(storage.getValue(path2), vec2);
    }
    std::remove(file_name.c_str());
}


TEST(NaiveStorageTest, UnexistingItemGetCase)
{
    std::string file_name = "file";
    std::string path = "/";
    NaiveStorage storage(file_name);
    EXPECT_THROW(storage.getValue(path), NoSuchPathException);
    std::remove(file_name.c_str());
}

TEST(NaiveStorageTest, OverwriteItemCase)
{
    std::string file_name = "file";
    std::string path = "/";
    std::vector<uint8_t> vec1{1, 2, 3, 4};
    std::vector<uint8_t> vec2{100, 1, 200, 2, 230, 3};

    {
        NaiveStorage storage(file_name);
        storage.setValue(path, vec1);
    }
    {
        NaiveStorage storage(file_name);
        storage.setValue(path, vec2);
    }
    {
        NaiveStorage storage(file_name);
        EXPECT_EQ(storage.getValue(path), vec2);
    }
    std::remove(file_name.c_str());
}