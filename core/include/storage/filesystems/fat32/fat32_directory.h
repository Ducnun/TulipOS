#pragma once

#include <stdint.h>

#include "hal/iblock_device.h"
#include "fat32_clustertable.h"

#define FAT_DIR_ENTRY_SIZE 32

// FAT Attributes
#define FAT_ATTR_READ_ONLY 0x01
#define FAT_ATTR_HIDDEN    0x02
#define FAT_ATTR_SYSTEM    0x04
#define FAT_ATTR_VOLUME_ID 0x08
#define FAT_ATTR_DIRECTORY 0x10
#define FAT_ATTR_ARCHIVE   0x20
#define FAT_ATTR_LFN       0x0F // Long File Name entry

#pragma pack(push, 1)
// Struct that adheres to the physical layout
struct RawFATDirectoryEntry {
    char name[11];          // 8 bytes name, 3 bytes extension
    uint8_t attr;
    uint8_t ntRes;          // Reserved for NT
    uint8_t crtTimeTecl;    // Create time (10ms units)
    uint16_t crtTime;
    uint16_t crtDate;
    uint16_t lstAccDate;
    uint16_t clusterHigh;   // High 16 bits of cluster (FAT32 only)
    uint16_t wrtTime;
    uint16_t wrtDate;
    uint16_t clusterLow;    // Low 16 bits of cluster
    uint32_t fileSize;
};
#pragma pack(pop)

// Struct that adheres to the logical layout
#pragma pack(push, 1)
struct FAT32DirectoryEntry {
    char name[9];
    char extension[4];
    uint8_t attributes;
    uint32_t firstCluster;
    uint32_t fileSize;
};
#pragma pack(pop)

class FAT32Directory {
public:
    FAT32Directory(IBlockDevice& device, FAT32ClusterTable& clusters, uint32_t firstCluster);
    bool next(FAT32DirectoryEntry& entry);

private:
    bool readEntry(FAT32DirectoryEntry& entry);

    IBlockDevice& _device;
    FAT32ClusterTable& _clustertable;

    uint32_t _firstCluster;
    uint32_t _currentCluster;

    uint32_t _sectorIndex;
    uint32_t _entryIndex;

    uint32_t _loadedSector; 
    alignas(4) uint8_t  _sectorBuffer[512];     // Force align so quick 32 bit reads don't mess up struct cast
};