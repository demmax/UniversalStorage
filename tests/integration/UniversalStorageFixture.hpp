//
// Created by maxon on 10.05.18.
//

#ifndef UNIVERSALSTORAGE_UNIVERSALSTORAGEFIXTURE_H
#define UNIVERSALSTORAGE_UNIVERSALSTORAGEFIXTURE_H

#include <gtest/gtest.h>
#include "StorageAccessor.hpp"
#include "CachingProxyStorage.h"
#include "NaiveStorage.h"


class StorageFixture : public ::testing::Test
{
protected:
    StorageAccessor defaultAccessor()
    {
        auto phys_storage = std::make_shared<NaiveStorage>("tmp");
        auto proxy_storage = std::make_shared<CachingProxyStorage>(phys_storage, 1024 * 1024 * 50);
        StorageAccessor accessor;
        accessor.mountPhysicalVolume(proxy_storage, "/", 100);
        return accessor;
    }
};

#endif //UNIVERSALSTORAGE_UNIVERSALSTORAGEFIXTURE_H
