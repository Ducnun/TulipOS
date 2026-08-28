#pragma once

#include <stdint.h>

#include "storage/block/iblock_device.h"
#include "fat32_clustertable.h"

struct FAT32DirectoryEntry {
    char name[9];
    char extension[4];

    uint8_t attributes;

    uint32_t firstCluster;
    uint32_t fileSize;
};

class FAT32Directory {
public:
    FAT32Directory(IBlockDevice& device, FAT32ClusterTable& clusters, uint32_t firstCluster);
    bool next(FAT32DirectoryEntry& entry);

private:
    bool readEntry(FAT32DirectoryEntry& entry);

    IBlockDevice& _device;
    FAT32ClusterTable& _clusters;

    uint32_t _firstCluster;
    uint32_t _currentCluster;

    uint32_t _sectorIndex;
    uint32_t _entryIndex;
};