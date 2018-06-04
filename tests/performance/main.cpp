//
// Created by maxon on 02.06.18.
//

#include <iostream>
#include <chrono>

#include "StorageAccessor.hpp"
#include "MappedFileBlockManager.h"
#include "BPTreeStorage.h"

using namespace UniversalStorage;
using namespace std;
using namespace std::chrono;

int main(int argc, char **argv)
{
    const std::string fileName = "test";
    const size_t dataCount = 50000;//50000000;

    auto blockManager = make_shared<MappedFileBlockManager>(fileName);
    auto storage = make_shared<BPTreeStorage>(blockManager);
    auto accessor = make_shared<StorageAccessor>();

    accessor->mountPhysicalVolume(storage, "/", 1);

    cout << "Start inserting test..." << endl;
    auto start = steady_clock::now();
    for (size_t i = 0u; i < dataCount; i++) {
        auto path = "/" + std::to_string(i);
        accessor->setValue(path, path);
    }
    auto end = steady_clock::now();
    auto d = duration_cast<seconds>( end - start ).count();
    cout << "Inserting " + to_string(dataCount) << " values: " << d << " seconds" << endl;
    cout << "average: " << duration_cast<milliseconds>(end - start).count() / dataCount << " ms" << endl << endl;



    cout << "Start getting test..." << endl;
    start = steady_clock::now();
    for (size_t i = 0u; i < dataCount; i++) {
        auto path = "/" + std::to_string(i);
        assert(accessor->getValue<std::string>(path) == path);
    }
    end = steady_clock::now();
    d = duration_cast<seconds>( end - start ).count();
    cout << "Getting " + to_string(dataCount) << " values: " << d << " seconds" << endl;
    cout << "average: " << duration_cast<milliseconds>(end - start).count() / dataCount  << " ms" << endl << endl;



    cout << "Start removing test..." << endl;
    start = steady_clock::now();
    for (size_t i = 0u; i < dataCount; i++) {
        auto path = "/" + std::to_string(i);
        accessor->removeValue(path);
    }
    end = steady_clock::now();
    d = duration_cast<seconds>( end - start ).count();
    cout << "Removing " + to_string(dataCount) << " values: " << d << " seconds" << endl;
    cout << "average: " << duration_cast<milliseconds>(end - start).count() / dataCount  << " ms" << endl << endl;

    std::remove(fileName.c_str());
    return 0;
}