//
// Created by maxon on 24.05.18.
//

#include <thread>
#include <fstream>

#include "UniversalStorageFixture.hpp"
#include "mocks/MemoryStorage.h"


using namespace UniversalStorage;



static const size_t FULL_CYCLE_COUNT = 5000000;
static const size_t MEASURE_CYCLE_COUNT = 10000;


TEST_F(StorageFixture, PerformanceTest)
{
    auto accessor = defaultAccessor();
    std::ofstream file("/tmp/mesurements");

    size_t outer_count = FULL_CYCLE_COUNT / MEASURE_CYCLE_COUNT;
    std::string data(1024 * 2, 'ё');
    for (auto i = 0; i < outer_count; i++) {
        auto start = std::chrono::steady_clock::now();
        for (auto j = 0; j < MEASURE_CYCLE_COUNT; j++) {
            accessor->setValue("/", data);
        }
        auto end = std::chrono::steady_clock::now();
        file << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
    }
}