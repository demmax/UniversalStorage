//
// Created by maxon on 29.04.18.
//

#include "../mocks/MemoryStorage.h"
#include "../mocks/MockPhysicalStorage.h"

#include <gtest/gtest.h>
#include <NaiveStorage.h>
#include <numeric>
#include <StorageAccessor.hpp>
#include <thread>

using namespace UniversalStorage;


StorageAccessorPtr makeSimpleAccessor()
{
    IStoragePtr storage = std::make_shared<MemoryStorage>();
    auto accessor = std::make_shared<StorageAccessor>();
    accessor->mountPhysicalVolume(storage, "/", 1);
    return accessor;
}


TEST(StorageAccessorFunctionality, SimpleIntegerSetGetCheck)
{
    auto path = "/";
    auto accessor = makeSimpleAccessor();
    uint32_t val = 523412112;
    accessor->setValue(path, val);
    EXPECT_EQ(accessor->getValue<decltype(val)>(path), val);
}


TEST(StorageAccessorFunctionality, SimpleIntegerDoubleSetCheck)
{
    auto path = "/";
    auto accessor = makeSimpleAccessor();
    uint32_t val = 523412112;
    accessor->setValue(path, val);
    val = 982732132;
    accessor->setValue(path, val);
    EXPECT_EQ(accessor->getValue<decltype(val)>(path), val);
}


TEST(StorageAccessorFunctionality, SimpleDoubleSetGetCheck)
{
    auto path = "/";
    auto accessor = makeSimpleAccessor();
    double val = 123123123.123;
    accessor->setValue(path, val);
    EXPECT_EQ(accessor->getValue<decltype(val)>(path), val);
}

TEST(StorageAccessorFunctionality, SimpleStringSetGetCheck)
{
    auto path = "/1";
    auto accessor = makeSimpleAccessor();
    std::string val{"I'am String!"};
    accessor->setValue(path, val);
    EXPECT_EQ(accessor->getValue<decltype(val)>(path), val);
}

TEST(StorageAccessorFunctionality, SimpleNonAsciiStringSetGetCheck)
{
    auto path = "/1";
    auto accessor = makeSimpleAccessor();
    std::string val{"Частично-двухбайтная строка с ё."};
    accessor->setValue(path, val);
    EXPECT_EQ(accessor->getValue<decltype(val)>(path), val);
}


TEST(StorageAccessorFunctionality, SimpleVectorSetGetCheck)
{
    auto path = "/1";
    auto accessor = makeSimpleAccessor();
    std::vector<uint8_t> data { 1, 2, 3, 4, 0 };
    accessor->setValue(path, data);
    EXPECT_EQ(accessor->getValue<decltype(data)>(path), data);
}


TEST(StorageAccessorFunctionality, SimpleCNullTerminatedStringSetGetCheck)
{
    auto path = "/1";
    auto accessor = makeSimpleAccessor();
    const char* val = "I'am C Null-Terminated String!";
    accessor->setValue(path, val);
    EXPECT_EQ(accessor->getValue<decltype(val)>(path), val);
}

TEST(StorageAccessorFunctionality, SimpleCNullTerminatedStringAsCharArraySetGetCheck)
{
    auto path = "/1";
    auto accessor = makeSimpleAccessor();
    char val[] = "I'am C Null-Terminated String!";
    accessor->setValue(path, val);
//    EXPECT_EQ(accessor->getValue<decltype(val)>(path), val);
}

TEST(StorageAccessorFunctionality, RemoveValueExpectStorageRemoving)
{
    auto path = "/1";
    auto storage1 = std::make_shared<MockPhysicalStorage>();
    auto storage2 = std::make_shared<MockPhysicalStorage>();
    StorageAccessor accessor;
    accessor.mountPhysicalVolume(storage1, "/", 1);
    accessor.mountPhysicalVolume(storage2, "/", 2);


    std::vector<uint8_t> data { 1, 2, 3, 4, 0 };
    accessor.setValue(path, data);

    EXPECT_CALL(*storage1, removeValue(path)).Times(1);
    EXPECT_CALL(*storage2, removeValue(path)).Times(1);
    accessor.removeValue(path);
}


TEST(StorageAccessorFunctionality, CustomTypeTest)
{
    struct A
    {
        int counter {0};
        A() = default;
        A(const A& o) {
            counter = counter + 1;
        }

        bool operator==(const A &o) const { return counter == o.counter; }
        std::vector<uint8_t> serialize() const {
            return std::vector<uint8_t> {1, 2, 3};
        }
        static A deserialize(const std::vector<uint8_t> &data) {
            A a;
            a.counter = std::accumulate(data.begin(), data.end(), 0);
            return a;
        }
    };

    auto path = "/1";
    A val; // {.i = 42, .f = 3.14, .s = "Строка", .a=1}
    auto accessor = makeSimpleAccessor();
    accessor->setValue(path, val);
    auto a = accessor->getValue<decltype(val)>(path);

    EXPECT_EQ(a.counter, 6);
}


struct CustomStruct
{
    int counter {0};
    CustomStruct() = default;
    CustomStruct(const CustomStruct& o) {
        counter = counter + 1;
    }

    bool operator==(const CustomStruct &o) const { return counter == o.counter; }
};

namespace UniversalStorage {
    namespace TypeTraits {
        template<>
        std::vector<uint8_t> serialize<CustomStruct>(const CustomStruct &a) {
            return std::vector<uint8_t> {1, 2, 3};
        }

        template<>
        CustomStruct deserialize<CustomStruct>(const std::vector<uint8_t> &data) {
            CustomStruct a;
            a.counter = std::accumulate(data.begin(), data.end(), 0);
            return a;
        }
    }
}

TEST(StorageAccessorFunctionality, CustomTypeWithFreeFunctionsTest)
{
    auto path = "/1";
    CustomStruct val; // {.i = 42, .f = 3.14, .s = "Строка", .s=1}
    auto accessor = makeSimpleAccessor();
    accessor->setValue(path, val);
    auto s = accessor->getValue<decltype(val)>(path);

    EXPECT_EQ(s.counter, 6);
}


TEST(StorageAccessorFunctionality, KeyTimeoutExistBeforeTest)
{
    auto accessor = makeSimpleAccessor();
    accessor->setValue("/", 42, std::chrono::seconds(1));
    std::this_thread::sleep_for(std::chrono::milliseconds(900));
    EXPECT_EQ(accessor->getValue<int>("/"), 42);
}

TEST(StorageAccessorFunctionality, KeyTimeoutNotExistAfterTest)
{
    auto accessor = makeSimpleAccessor();
    accessor->setValue("/", 42, std::chrono::seconds(1));
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    EXPECT_THROW(accessor->getValue<int>("/"), NoSuchPathException);
}