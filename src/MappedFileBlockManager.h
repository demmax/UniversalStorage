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
    ~MappedFileBlockManager();

    uint8_t *getRootBlock() override;
    uint8_t *getFreeTreeNodeBlock() override;
    void freeTreeNodeBlock(uint64_t offset) override;

    uint8_t *getBlockPointer(uint64_t offset) override;
    uint8_t *getFreeBlock() override;

    uint64_t storeData(const std::vector<uint8_t> &data, uint64_t offset) override;
    uint64_t storeNewData(const std::vector<uint8_t> &data) override;
    uint64_t storePath(const std::string &path, uint64_t offset) override;
    uint64_t storeNewPath(const std::string &path) override;
    void freeBlock(uint64_t offset) override;
    std::string getPathFromBlock(uint64_t offset) override;
    std::vector<uint8_t> getDataFromBlock(uint64_t offset) override;

    bool isRootInitialized() const override;
    uint64_t getRootOffset() const override ;
    uint64_t getOffset(uint8_t *address) const override;


protected:
    static constexpr uint64_t END_BLOCKS = std::numeric_limits<uint64_t>::max();

    uint64_t findFreeBlockOffset(uint64_t min_offset = 0);
    uint64_t findContiguousFreeBlocks(uint64_t count, uint64_t pos = 0);

    uint64_t storeData(const uint8_t *data, uint64_t data_size, uint64_t offset);

    bool isBlockFree(uint64_t offset) const;
    void aquireBlock(uint64_t offset);
    uint64_t getSectorNumber(uint64_t offset) const;
    uint64_t getBlockNumber(uint64_t offset) const;
    int getFreeBlockIndex(uint8_t *bitmap, uint64_t start_index = 0) const;

    void resizeFile(uint64_t new_size);

    boost::iostreams::mapped_file m_mappedFile;
    uint64_t m_firstFreeBlockOffset;
    uint32_t m_version { 1 };
    uint8_t *p_rootNodeBlock;
    uint8_t *p_firstDataBlock; //!< after header
    bool m_rootInitialized { true };

    static constexpr size_t HEADER_SIZE = 4 + 8 + 8; // version + reserved place + first free block offset
    static constexpr size_t BITMAP_SIZE = DATA_BLOCK_SIZE;
    static constexpr size_t SECTOR_DATA_SIZE = BITMAP_SIZE + (BITMAP_SIZE * 8  * DATA_BLOCK_SIZE);
    static constexpr size_t DATA_LENGTH_SIZE = 8;
    static constexpr size_t NEXT_DATA_SIZE = 8;
    static constexpr size_t PLAIN_BLOCK_DATA_SIZE = DATA_BLOCK_SIZE - DATA_LENGTH_SIZE - NEXT_DATA_SIZE;

    uint64_t blocksCount {0};
    uint64_t sectorsCount {0};
};

}

#endif //UNIVERSALSTORAGE_MAPPEDFILEBLOCKMANAGER_H
