//
// Created by maxon on 04.05.18.
//

#include <gtest/gtest.h>
#include "NaiveStorage.h"


TEST(SimpleSetGetCase, NaiveStorageTests)
{
    NaiveStorage storage("/tmp/file");
    std::string path = "/";
    std::vector<uint8_t> vec{1, 2, 3, 4};
    storage.setValue(path, vec);
    EXPECT_EQ(storage.getValue(path), vec);
}
