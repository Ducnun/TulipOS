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

    if (_bytesPerSector != 512) return false;
    if (_sectorsPerCluster == 0 || _sectorsPerCluster > 128) return false;
    if ((_sectorsPerCluster & (_sectorsPerCluster - 1)) != 0) return false;
    if (_reservedSectors == 0) return false;
    if (_numberOfFats == 0) return false;           // Can't have 0 FATs
    if (_fatSize == 0) return false;                // Fatsize can not be 0
    if (_rootCluster < 2) return false;             // Root cluster must start above 2

    return true;
}