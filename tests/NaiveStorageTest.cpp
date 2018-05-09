//
// Created by maxon on 04.05.18.
//

#include <gtest/gtest.h>
#include <exceptions.h>
#include "NaiveStorage.h"


TEST(SimpleSetGetCase, NaiveStorageTest)
{
    size_t count = 5000;
    std::string file_name = "file";
    NaiveStorage storage(file_name);
    std::string path = "/";
    std::vector<uint8_t> vec{1, 2, 3, 4};
    for (int i = 0; i < count; i++) {
        std::vector v = vec;
        v.push_back(i);
        storage.setValue(path + std::to_string(i), v);
    }

    for (size_t i = 0; i < count; i++) {
        auto expected_vec = vec;
        expected_vec.push_back(i % 256);
        EXPECT_EQ(storage.getValue(path + std::to_string(i)), expected_vec);
    }
    std::remove(file_name.c_str());
}


TEST(ConnectingToStorageSetGetCase, NaiveStorageTest)
{
    std::string file_name = "file1";
    size_t count = 5000;
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


TEST(SimpleRemoveItemStorageTest, NaiveStorageTest)
{
    std::string file_name = "file";
    NaiveStorage storage(file_name);
    std::string path = "/";
    std::vector<uint8_t> vec{1, 2, 3, 4};

    storage.setValue(path, vec);
    storage.removeValue(path);
    EXPECT_THROW(storage.getValue(path), NoSuchPathException);
}


TEST(RemoveExactlyOneItemStorageTest, NaiveStorageTest)
{
    std::string file_name = "file";
    NaiveStorage storage(file_name);
    std::string path1 = "/";
    std::string path2 = "/a";
    std::vector<uint8_t> vec1{1, 2, 3, 4};
    std::vector<uint8_t> vec2{5, 6, 7};

    storage.setValue(path1, vec1);
    storage.setValue(path2, vec2);

    storage.removeValue(path1);
    EXPECT_EQ(storage.getValue(path2), vec2);
}


TEST(UnexistingItemGetCase, NaiveStorageTest)
{
    std::string file_name = "file";
    NaiveStorage storage(file_name);
    std::string path = "/";
    EXPECT_THROW(storage.getValue(path), NoSuchPathException);
}