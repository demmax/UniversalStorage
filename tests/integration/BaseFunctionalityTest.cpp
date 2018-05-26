//
// Created by maxon on 10.05.18.
//

#include "UniversalStorageFixture.hpp"

using namespace UniversalStorage;


TEST_F(StorageFixture, BasicFunctionalityTest)
{
    auto accessor = defaultAccessor();
    accessor->setValue("/a/b/c", 42);
    accessor->setValue("/a/b", "Hello, World!");
    accessor->setValue("/a", 3.14);
    accessor->setValue("/a/z", std::string("Hello from std::string!"));

    EXPECT_EQ(accessor->getValue<uint32_t>("/a/b/c"), 42);
    EXPECT_EQ(accessor->getValue<const char*>("/a/b"), "Hello, World!");
    EXPECT_EQ(accessor->getValue<double>("/a"), 3.14);
    EXPECT_EQ(accessor->getValue<std::string>("/a/z"), "Hello from std::string!");
}
