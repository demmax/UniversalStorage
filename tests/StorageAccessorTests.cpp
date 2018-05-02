//
// Created by maxon on 29.04.18.
//

#include "StorageAccessor.h"
#include "mocks/NaivePhysicalStorage.h"
#include "mocks/MockPhysicalStorage.h"

#include <gtest/gtest.h>


StorageAccessor makeSimpleAccessor()
{
    IStoragePtr storage = std::make_shared<NaivePhysicalStorage>();
    StorageAccessor accessor;
    accessor.mountPhysicalVolume(storage, "/", 1);
    return std::move(accessor);
}


TEST(SimpleIntegerSetGetCheck, StorageAccessorFunctionality)
{
    auto path = "/";
    auto accessor = makeSimpleAccessor();
    uint32_t val = 523412112;
    accessor.setValue(path, val);
    EXPECT_EQ(accessor.getValue<decltype(val)>(path), val);
}


TEST(SimpleIntegerDoubleSetCheck, StorageAccessorFunctionality)
{
    auto path = "/";
    auto accessor = makeSimpleAccessor();
    uint32_t val = 523412112;
    accessor.setValue(path, val);
    val = 982732132;
    accessor.setValue(path, val);
    EXPECT_EQ(accessor.getValue<decltype(val)>(path), val);
}


TEST(SimpleDoubleSetGetCheck, StorageAccessorFunctionality)
{
    auto path = "/";
    auto accessor = makeSimpleAccessor();
    double val = 123123123.123;
    accessor.setValue(path, val);
    EXPECT_EQ(accessor.getValue<decltype(val)>(path), val);
}

TEST(SimpleStringSetGetCheck, StorageAccessorFunctionality)
{
    auto path = "/1";
    auto accessor = makeSimpleAccessor();
    std::string val{"I'am String!"};
    accessor.setValue(path, val);
    EXPECT_EQ(accessor.getValue<decltype(val)>(path), val);
}

TEST(SimpleNonAsciiStringSetGetCheck, StorageAccessorFunctionality)
{
    auto path = "/1";
    auto accessor = makeSimpleAccessor();
    std::string val{"Частично-двухбайтная строка с ё."};
    accessor.setValue(path, val);
    EXPECT_EQ(accessor.getValue<decltype(val)>(path), val);
}


TEST(SimpleVectorSetGetCheck, StorageAccessorFunctionality)
{
    auto path = "/1";
    auto accessor = makeSimpleAccessor();
    std::vector<uint8_t> data { 1, 2, 3, 4, 0 };
    accessor.setValue(path, data);
    EXPECT_EQ(accessor.getValue<decltype(data)>(path), data);
}


TEST(SelectRightStorage, PhysicalVolumeSelector)
{

}