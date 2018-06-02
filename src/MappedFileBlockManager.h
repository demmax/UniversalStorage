//
// Created by maxon on 26.05.18.
//

#ifndef UNIVERSALSTORAGE_MAPPEDFILEBLOCKMANAGER_H
#define UNIVERSALSTORAGE_MAPPEDFILEBLOCKMANAGER_H

#include <boost/iostreams/device/mapped_file.hpp>
#include "IBlockManager.h"


namespace UniversalStorage {


class MappedFileBlockManager : public IBlockManager
{
public:
    MappedFileBlockManager(const std::string &file_name);

    uint8_t *getRootBlock() override;
    uint8_t *getTreeNodeBlock(uint64_t offset) override;
    uint8_t *getFreeTreeNodeBlock() override;
    void freeTreeNodeBlock(uint8_t *address) override;

    uint64_t storeData(const std::vector<uint8_t> &data, uint64_t offset) override;
    uint64_t storeNewData(const std::vector<uint8_t> &data) override;
    uint64_t storePath(const std::string &path, uint64_t offset) override;
    uint64_t storeNewPath(const std::string &path) override;
    void freeBlock(uint64_t offset) override;
    std::string getPathFromBlock(uint64_t offset) override;
    std::vector<uint8_t> getDataFromBlock(uint64_t offset) override;

    bool isRootInitialized() const override;
    uint64_t getOffset(uint8_t *address) override;

protected:
    boost::iostreams::mapped_file m_mappedFile;
};

}

#endif //UNIVERSALSTORAGE_MAPPEDFILEBLOCKMANAGER_H
