//
// Created by maxon on 10.05.18.
//

#ifndef UNIVERSALSTORAGE_UNIVERSALSTORAGEFIXTURE_H
#define UNIVERSALSTORAGE_UNIVERSALSTORAGEFIXTURE_H

#include <gtest/gtest.h>
#include "StorageAccessor.hpp"
#include "LRUCachingProxyStorage.h"
#include "NaiveStorage.h"

using namespace UniversalStorage;


class StorageFixture : public ::testing::Test
{
protected:
    static constexpr const char *FILE_NAME = "tmp";
    StorageAccessorPtr defaultAccessor()
    {
        auto phys_storage = std::make_shared<NaiveStorage>(FILE_NAME);
        auto proxy_storage = std::make_shared<LRUCachingProxyStorage>(phys_storage, 1024 * 1024 * 50);
        auto accessor = std::make_shared<StorageAccessor>();
        accessor->mountPhysicalVolume(proxy_storage, "/", 100);
        return accessor;
    }

    ~StorageFixture() {
        std::remove(FILE_NAME);
    }
};

#endif //UNIVERSALSTORAGE_UNIVERSALSTORAGEFIXTURE_H
