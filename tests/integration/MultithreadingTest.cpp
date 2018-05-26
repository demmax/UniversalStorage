//
// Created by maxon on 22.05.18.
//

#include <thread>

#include "UniversalStorageFixture.hpp"
#include "mocks/MemoryStorage.h"


using namespace UniversalStorage;



static const size_t CYCLE_COUNT = 10000;


TEST_F(StorageFixture, MultithreadingAddRemoveStorageTest)
{
    std::atomic_size_t inserted = {0};

    StorageAccessor accessor;
    std::vector<IStoragePtr> storages;
    for (size_t i = 0; i < CYCLE_COUNT; ++i) {
        storages.emplace_back(std::make_shared<MemoryStorage>());
    }

    std::thread t1([&]() {
        for (size_t i = 0; i < CYCLE_COUNT; ++i) {
            accessor.mountPhysicalVolume(storages[i], "/" + std::to_string(i), 1);
            ++inserted;
        }
    } );

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::thread t2([&]() {
        for (size_t i = 0; i < CYCLE_COUNT; ++i) {
            while (inserted <= i) ;
            accessor.umountVolume("/" + std::to_string(i));
        }
    } );

    t1.join();
    t2.join();

    EXPECT_TRUE(true);
}

TEST_F(StorageFixture, MultithreadingTest)
{
//    auto accessor = defaultAccessor();
//    std::thread t1([&]() {
//        for (size_t i = 0; i < CYCLE_COUNT; ++i) {
//            accessor->setValue("/a/b", 42);
//        }
//    } );
//
//    std::thread t2([&]() {
//        for (size_t i = 0; i < CYCLE_COUNT; ++i) {
//            accessor->setValue("/a/b", "Hello, World!");
//        }
//    } );
//
//    std::thread t3([&]() {
//        for (size_t i = 0; i < CYCLE_COUNT; ++i) {
//            accessor->setValue("/a/b", 3.14);
//        }
//    } );
//
//    std::thread t4([&]() {
//        for (size_t i = 0; i < CYCLE_COUNT; ++i) {
//            accessor->setValue("/a/b", "Hello, World!");
//        }
//    } );
//
//    t1.join();
//    t2.join();
//    t3.join();
//    t4.join();

}