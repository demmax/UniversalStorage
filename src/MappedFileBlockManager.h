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
    int getFreeBlockIndex() const;

    boost::iostreams::mapped_file m_mappedFile;
    uint64_t m_firstFreeBlockOffset;
    uint32_t m_version;
    uint8_t *p_rootNodeBlock;
    uint8_t *p_firstDataBlock; //!< after header

    static constexpr size_t HEADER_SIZE = 4 + 8 + 8; // version + reserved place + first free block offset
    static constexpr size_t BITMAP_SIZE = DATA_BLOCK_SIZE;
    static constexpr size_t SECTOR_DATA_SIZE = BITMAP_SIZE + (BITMAP_SIZE * 8  * DATA_BLOCK_SIZE); // bitmap + blocks
};

}

#endif //UNIVERSALSTORAGE_MAPPEDFILEBLOCKMANAGER_H
