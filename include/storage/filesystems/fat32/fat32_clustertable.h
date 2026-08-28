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
    FAT32ClusterTable(IBlockDevice& device, const FAT32BPB& bpb) : 
        _device(device), _bpb(bpb) {
            _fatStartSector = _bpb.reservedSectors();
            _dataStartSector = _bpb.reservedSectors() + (_bpb.numberOfFats() * _bpb.fatSize());
        }

    uint32_t firstSector(uint32_t cluster) const {
        return _dataStartSector + ((cluster - 2) * _bpb.sectorsPerCluster());       // Cluster numbers 0 and 1 are reserved (FAT32 reasons)
    }

    uint32_t nextCluster(uint32_t cluster) {
        uint32_t fatOffset = cluster * 4;   // 32 bit lookups

        // Get sector and offset inside of sector (remainder of first calc)
        uint32_t fatSector = _fatStartSector + fatOffset / _bpb.bytesPerSector();
        uint32_t offsetInSector = fatOffset % _bpb.bytesPerSector();

        // Retrieve sector that holds index of cluster
        uint8_t sector[512];
        if (!_device.readBlock(fatSector, sector)) return FAT32_CLUSTER_INVALID;

        // Read out exact cluster index using offset inside of sector. Mask out upper nibble (FAT32 reasons)
        return util::readLE32(&sector[offsetInSector]) & 0x0FFFFFFF;
    }

    bool isEndOfChain(uint32_t value) {
        return (value >= FAT32_CLUSTER_EOC);
    }

private:
    IBlockDevice& _device;
    const FAT32BPB& _bpb;

    uint32_t _fatStartSector;
    uint32_t _dataStartSector;
};