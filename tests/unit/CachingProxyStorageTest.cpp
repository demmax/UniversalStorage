//
// Created by maxon on 08.05.18.
//

#include <gtest/gtest.h>
#include "../mocks/MockPhysicalStorage.h"
#include "CachingProxyStorage.h"

TEST(SimpleGetCaseExpectStorageAccess, CachingStorageTest)
{
    std::string path = "/a/b/c";
    auto mock_storage = std::make_shared<MockPhysicalStorage>();
    CachingProxyStorage cache(mock_storage, 1024);

    EXPECT_CALL(*mock_storage, getValue(path)).Times(1);
    cache.getValue(path);
}

TEST(SimpleTestSetGetCaseExpectOneStorageWriteZeroRead, CachingStorageTest)
{
    std::string path = "/a/b/c";
    std::vector<uint8_t> vec = {1, 2, 3};
    auto mock_storage = std::make_shared<MockPhysicalStorage>();
    CachingProxyStorage cache(mock_storage, 1024);

    EXPECT_CALL(*mock_storage, setValue(path, vec));
    EXPECT_CALL(*mock_storage, getValue(testing::_)).Times(0);
    cache.setValue(path, vec);
    EXPECT_EQ(cache.getValue(path), vec);
}

TEST(SimpleTestFewSetGetCase, CachingStorageTest)
{
    std::string path1 = "/a/b/c";
    std::string path2 = "/d/e/f";
    std::string path3 = "/g/h/i";

    std::vector<uint8_t> vec1 = {1, 2, 3};
    std::vector<uint8_t> vec2 = {4, 5, 6};
    std::vector<uint8_t> vec3 = {7, 8, 9};

    auto mock_storage = std::make_shared<MockPhysicalStorage>();
    CachingProxyStorage cache(mock_storage, 1024);

    EXPECT_CALL(*mock_storage, setValue(path1, vec1));
    EXPECT_CALL(*mock_storage, setValue(path2, vec2));
    EXPECT_CALL(*mock_storage, setValue(path3, vec3));
    EXPECT_CALL(*mock_storage, getValue(testing::_)).Times(0);

    cache.setValue(path1, vec1);
    cache.setValue(path2, vec2);
    cache.setValue(path3, vec3);
    EXPECT_EQ(cache.getValue(path1), vec1);
    EXPECT_EQ(cache.getValue(path2), vec2);
    EXPECT_EQ(cache.getValue(path3), vec3);
    EXPECT_EQ(cache.getValue(path1), vec1);
    EXPECT_EQ(cache.getValue(path2), vec2);
    EXPECT_EQ(cache.getValue(path3), vec3);
}

TEST(CacheRotateExpectStorageAccess, CachingStorageTest)
{
    std::string path1 = "/a/b/c";
    std::string path2 = "/d/e/f";
    std::string path3 = "/g/h/i";

    std::vector<uint8_t> vec1 = {1, 2, 3};
    std::vector<uint8_t> vec2 = {4, 5, 6};
    std::vector<uint8_t> vec3 = {7, 8, 9};

    auto mock_storage = std::make_shared<MockPhysicalStorage>();
    CachingProxyStorage cache(mock_storage, 8);

    EXPECT_CALL(*mock_storage, setValue(path1, vec1));
    EXPECT_CALL(*mock_storage, setValue(path2, vec2));
    EXPECT_CALL(*mock_storage, setValue(path3, vec3));
    EXPECT_CALL(*mock_storage, getValue(path1)).WillOnce(::testing::Return(vec1));
    cache.setValue(path1, vec1);
    cache.setValue(path2, vec2);
    cache.setValue(path3, vec3);
    EXPECT_EQ(cache.getValue(path3), vec3);
    EXPECT_EQ(cache.getValue(path2), vec2);
    EXPECT_EQ(cache.getValue(path1), vec1);
}

TEST(CacheDoubleSetCaseExpectLastValue, CachingStorageTest)
{
    std::string path = "/a/b/c";
    std::vector<uint8_t> vec1 = {1, 2, 3};
    std::vector<uint8_t> vec2 = {4, 5, 6};

    auto mock_storage = std::make_shared<MockPhysicalStorage>();
    CachingProxyStorage cache(mock_storage, 8);
    EXPECT_CALL(*mock_storage, setValue(::testing::_, ::testing::_)).Times(2); // suppress gmock warning about "Uninteresting mock function call - returning directly"
    cache.setValue(path, vec1);
    cache.setValue(path, vec2);
    EXPECT_EQ(cache.getValue(path), vec2);
}

TEST(CacheDoubleGetAfterCacheClearingCaseExpectJustOneLoad, CachingStorageTest)
{
    std::string path1 = "/a/b/c";
    std::string path2 = "/d/e/f";
    std::string path3 = "/g/h/i";

    std::vector<uint8_t> vec1 = {1, 2, 3};
    std::vector<uint8_t> vec2 = {4, 5, 6};
    std::vector<uint8_t> vec3 = {7, 8, 9};

    auto mock_storage = std::make_shared<MockPhysicalStorage>();
    CachingProxyStorage cache(mock_storage, 8);

    EXPECT_CALL(*mock_storage, setValue(path1, vec1));
    EXPECT_CALL(*mock_storage, setValue(path2, vec2));
    EXPECT_CALL(*mock_storage, setValue(path3, vec3));
    EXPECT_CALL(*mock_storage, getValue(path1)).WillOnce(::testing::Return(vec1));
    cache.setValue(path1, vec1);
    cache.setValue(path2, vec2);
    cache.setValue(path3, vec3);
    EXPECT_EQ(cache.getValue(path3), vec3);
    EXPECT_EQ(cache.getValue(path2), vec2);
    EXPECT_EQ(cache.getValue(path1), vec1);
    EXPECT_EQ(cache.getValue(path1), vec1);
    EXPECT_EQ(cache.getValue(path1), vec1);
}

TEST(SimpleRemoveExpectStorageRemove, CachingStorageTest)
{
    std::string path = "/a/b/c";
    auto mock_storage = std::make_shared<MockPhysicalStorage>();
    CachingProxyStorage cache(mock_storage, 1024);

    EXPECT_CALL(*mock_storage, removeValue(path)).Times(1);
    cache.removeValue(path);
}