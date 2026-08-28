#pragma once

#include <stdint.h>
#include "utils/endian.h"

class FAT32BPB{
public:
    bool parse(uint8_t* sector){
        //  Check signature BPB end bytes
        if (sector[510] != 0x55 || sector[511] != 0xAA) return false;
        
        // Parse main BPB fields
        _bytesPerSector     = util::readLE16(sector + 0x0B);
        _sectorsPerCluster  = *(sector + 0x0D);
        _reservedSectors    = util::readLE16(sector + 0x0E);
        _numberOfFats       = *(sector + 0x10);

        // FAT32 specific fields
        _fatSize        = util::readLE32(sector + 0x24);
        _rootCluster    = util::readLE32(sector + 0x2C);

        // Validate some fields (e.g. powers of 2)
        if (_sectorsPerCluster % 2 != 0) return false;  // Sectors per cluster is power of 2
        if (_sectorsPerCluster > 128) return false;     // Max cluster size
        if (_bytesPerSector % 2 != 0) return false;     // Bytes per sector is power of 2
        if (_numberOfFats == 0) return false;           // Can't have 0 FATs
        if (_fatSize == 0) return false;                // Fatsize can not be 0
        if (_rootCluster < 2) return false;             // Root cluser must start above 2

        return true;
    }

    uint16_t bytesPerSector()       const { return _bytesPerSector; }
    uint8_t  sectorsPerCluster()    const { return _sectorsPerCluster; }
    uint16_t reservedSectors()      const { return _reservedSectors; }
    uint8_t  numberOfFats()         const { return _numberOfFats; }
    uint32_t fatSize()              const { return _fatSize; }
    uint32_t rootCluster()          const { return _rootCluster; }
private:
    uint16_t _bytesPerSector;
    uint8_t  _sectorsPerCluster;
    uint16_t _reservedSectors;
    uint8_t  _numberOfFats;
    uint32_t _fatSize;
    uint32_t _rootCluster;
};