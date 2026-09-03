#include "storage/filesystems/fat32/fat32_bpb.h"

bool FAT32BPB::parse(uint8_t* sector){
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

    // TODO: Add proper error codes and check sector size (strictly 512 bytes supported on our system)

    // Validate some fields (e.g. powers of 2)
    if (_sectorsPerCluster > 128) return false;     // Max cluster size
    if (_numberOfFats == 0) return false;           // Can't have 0 FATs
    if (_fatSize == 0) return false;                // Fatsize can not be 0
    if (_rootCluster < 2) return false;             // Root cluser must start above 2

    return true;
}