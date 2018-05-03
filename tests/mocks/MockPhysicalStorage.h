//
// Created by maxon on 29.04.18.
//

#ifndef UNIVERSALSTORAGE_MOCKPHYSICALSTORAGE_H
#define UNIVERSALSTORAGE_MOCKPHYSICALSTORAGE_H


#include "../../src/IStorage.h"
#include "../../src/IStorage.h"
#include <gmock/gmock.h>
#include <string>


class MockPhysicalStorage : public IStorage
{
public:
    MOCK_METHOD2(setValue, void(const std::string &path, const std::vector<uint8_t> &data));
    MOCK_CONST_METHOD1(getValue, std::vector<uint8_t>(const std::string &path));
    MOCK_CONST_METHOD1(isExist, bool(const std::string &path));
};

#endif //UNIVERSALSTORAGE_MOCKPHYSICALSTORAGE_H
