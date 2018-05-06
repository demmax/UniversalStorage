//
// Created by maxon on 04.05.18.
//

#include <gtest/gtest.h>
#include "NaiveStorage.h"


TEST(SimpleSetGetCase, NaiveStorageTests)
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


TEST(ConnectingToStorageSetGetCase, NaiveStorageTests)
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