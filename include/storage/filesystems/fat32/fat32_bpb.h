#pragma once

#include <stdint.h>
#include "utils/endian.h"

// Force allignment on 4 byte address
class alignas(4) FAT32BPB{
public:
    bool parse(uint8_t* sector);
    void debug() const;

    uint16_t bytesPerSector()       const { return _bytesPerSector; }
    uint8_t  sectorsPerCluster()    const { return _sectorsPerCluster; }
    uint16_t reservedSectors()      const { return _reservedSectors; }
    uint8_t  numberOfFats()         const { return _numberOfFats; }
    uint32_t fatSize()              const { return _fatSize; }
    uint32_t rootCluster()          const { return _rootCluster; }
private:
    uint32_t _fatSize;
    uint32_t _rootCluster;
    uint16_t _bytesPerSector;
    uint16_t _reservedSectors;
    uint8_t  _sectorsPerCluster;
    uint8_t  _numberOfFats;
};