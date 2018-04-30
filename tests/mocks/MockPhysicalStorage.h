//
// Created by maxon on 29.04.18.
//

#ifndef UNIVERSALSTORAGE_MOCKPHYSICALSTORAGE_H
#define UNIVERSALSTORAGE_MOCKPHYSICALSTORAGE_H


#include "IPhysicalStorage.h"
#include "../../src/IPhysicalStorage.h"
#include <gmock/gmock.h>


class MockPhysicalStorage : public IPhysicalStorage
{
public:
    MOCK_METHOD3(setValue, void(const std::string &path, const uint8_t *data, size_t s));
    MOCK_CONST_METHOD1(getValue, std::tuple<uint8_t*, size_t>(const std::string &path));
    MOCK_CONST_METHOD1(isExist, bool(const std::string &path));
};

#endif //UNIVERSALSTORAGE_MOCKPHYSICALSTORAGE_H
