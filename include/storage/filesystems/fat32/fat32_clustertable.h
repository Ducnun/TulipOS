#pragma once

#include <stdint.h>

#include "storage/block/iblock_device.h"
#include "storage/filesystems/fat32/fat32_bpb.h"

#define FAT32_CLUSTER_FREE       0x00000000
#define FAT32_CLUSTER_BAD        0x0FFFFFF7
#define FAT32_CLUSTER_EOC        0x0FFFFFF8
#define FAT32_CLUSTER_INVALID    0xFFFFFFFF

class FAT32ClusterTable {
public:
    FAT32ClusterTable(IBlockDevice& device, const FAT32BPB& bpb);

    uint32_t firstSector(uint32_t cluster) const {
        return _dataStartSector + ((cluster - 2) * _bpb.sectorsPerCluster());       // Cluster numbers 0 and 1 are reserved (FAT32 reasons)
    }

    uint32_t nextCluster(uint32_t cluster);

    bool isEndOfChain(uint32_t value) {
        return (value >= FAT32_CLUSTER_EOC);
    }

    const FAT32BPB bpb() const { return _bpb; };

private:
    IBlockDevice& _device;
    const FAT32BPB& _bpb;

    uint32_t _fatStartSector;
    uint32_t _dataStartSector;
};