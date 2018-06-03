//
// Created by maxon on 02.06.18.
//

#ifndef UNIVERSALSTORAGE_UTILS_H
#define UNIVERSALSTORAGE_UTILS_H

#include <string>
#include <vector>


namespace UniversalStorage {

struct Utils
{
    static uint64_t hash(const std::string &str);
    static uint64_t packVector(const std::vector<uint8_t> &vec);
    static std::vector<uint8_t> unpackValue(uint64_t data);
};


}

#endif //UNIVERSALSTORAGE_UTILS_H
