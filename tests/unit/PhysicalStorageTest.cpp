//
// Created by maxon on 26.05.18.
//


#include <gtest/gtest.h>
#include <numeric>
#include <thread>
#include <PhysicalStorage.h>
#include "StorageAllocator.h"

#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>

#include <boost/filesystem.hpp>


using namespace UniversalStorage;


TEST(PhysicalStorageTest, BaseTest)
{
//    StorageAllocator allocator("");
    namespace bio = boost::iostreams;
    using namespace std;
    vector<uint8_t> strArray(2000000);

//    bio::mapped_file_params params;
//    params.path          = "text.txt";
//    params.new_file_size = 1ul << 30u;
//    params.flags         = bio::mapped_file::mapmode::readwrite;

    if (!boost::filesystem::exists("text.txt")) {
        boost::filesystem::ofstream( "text.txt" );
        boost::filesystem::resize_file("text.txt", 1);
    }

    boost::iostreams::mapped_file mmap("text.txt", boost::iostreams::mapped_file::readwrite);
    std::cout << std::endl << "size: " << mmap.size() << std::endl;
    mmap.resize(100);
    std::cout << std::endl << "new size: " << mmap.size() << std::endl;
    mmap.resize(50);
    std::cout << std::endl << "new new size: " << mmap.size() << std::endl;
    char *data = mmap.data();
//    bio::stream<bio::mapped_file_sink> out(params);

//    copy(strArray.begin(), strArray.end(), ostream_iterator<uint8_t >(out, "\n"));
}