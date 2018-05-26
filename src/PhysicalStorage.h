//
// Created by maxon on 26.05.18.
//

#ifndef UNIVERSALSTORAGE_PHYSICALSTORAGE_H
#define UNIVERSALSTORAGE_PHYSICALSTORAGE_H

#include "IStorage.h"
#include "BPTree.h"


namespace UniversalStorage {


class PhysicalStorage : public IStorage
{
public:
    void setValue(const std::string &path, const std::vector<uint8_t> &data) override;
    std::vector<uint8_t> getValue(const std::string &path) const override;
    void removeValue(const std::string &path) override;
    bool isExist(const std::string &path) const override;

protected:
    BPTree btree;
};

}

#endif //UNIVERSALSTORAGE_PHYSICALSTORAGE_H
