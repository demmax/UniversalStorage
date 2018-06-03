//
// Created by maxon on 02.06.18.
//

#include <gtest/gtest.h>
#include "MappedFileBlockManager.h"
#include "utils.h"


using namespace UniversalStorage;

TEST(BlockManagerTest, SetGetPathTest)
{
    const std::string fileName = "/tmp/data";
    MappedFileBlockManager manager(fileName);

    auto offset = manager.storeNewPath("1");
    auto path = manager.getPathFromBlock(offset);
    EXPECT_EQ(path, "1");

    std::remove(fileName.c_str());
}


TEST(BlockManagerTest, SetGetDataTest)
{
    const std::string fileName = "/tmp/data";
    MappedFileBlockManager manager(fileName);

    std::vector<uint8_t> data_vector {1, 2, 3, 4 ,5 ,6, 7, 8, 9, 10};
    auto offset = manager.storeNewData(data_vector);
    EXPECT_EQ(manager.getDataFromBlock(offset), data_vector);

    std::remove(fileName.c_str());
}


TEST(BlockManagerTest, SetGetDataMoreThanOneSectorData)
{
    const std::string fileName = "/tmp/data";
    MappedFileBlockManager manager(fileName);

    std::vector<uint64_t> offsets;
    for (auto i = 0u; i < 10000; i++) {
        auto offset = manager.storeNewData(Utils::unpackValue(i));
        offsets.push_back(offset);
    }

    EXPECT_EQ(offsets.size(), 10000);

    for (auto offset : offsets) {
        EXPECT_EQ(manager.getDataFromBlock(offset), Utils::unpackValue(offset));
    }

    std::remove(fileName.c_str());
}