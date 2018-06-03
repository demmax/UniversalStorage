//
// Created by maxon on 02.06.18.
//

#include "utils.h"

using namespace UniversalStorage;

uint64_t Utils::hash(const std::string &str)
{
    std::hash<std::string> hash_fn;
    return hash_fn(str);
}


uint64_t Utils::packVector(const std::vector<uint8_t> &vec)
{
    uint64_t packed = 0;
    for (auto i = 0u; i < vec.size(); ++i) {
        packed |= (uint64_t)vec[i] << (i * 8);
    }
    return packed;
}


std::vector<uint8_t> Utils::unpackValue(uint64_t data)
{
    std::vector<uint8_t> result;
    result.reserve(8);
    for (auto i = 0u; i < 8; ++i) {
        result[i] = static_cast<uint8_t>(data >> (i * 8));
    }
    return result;
}