#include "storage/filesystems/fat32/fat32_directory.h"

FAT32Directory::FAT32Directory(IBlockDevice& device, FAT32ClusterTable& clusters, uint32_t firstCluster) : 
        _device(device),
        _clustertable(clusters),
        _firstCluster(firstCluster),
        _currentCluster(firstCluster),
        _sectorIndex(0),
        _entryIndex(0),
        _loadedSector(0xFFFFFFF)
    {}

bool FAT32Directory::next(FAT32DirectoryEntry& entry) {
    const uint32_t entriesPerSector = 512 / FAT_DIR_ENTRY_SIZE;
    
    // Start iterating through the chain
    while (!_clustertable.isEndOfChain(_currentCluster)) {
        uint32_t absoluteSector = _clustertable.firstSector(_currentCluster) + _sectorIndex;

        // Only read from block device if not loaded into RAM yet
        if (_loadedSector != absoluteSector) {
            if (!_device.readBlock(absoluteSector, _sectorBuffer)) return false;
            _loadedSector = absoluteSector;
        }

        // Scan through the entries
        while (_entryIndex < entriesPerSector) {
            RawFATDirectoryEntry* raw = (RawFATDirectoryEntry*)(_sectorBuffer + (_entryIndex * FAT_DIR_ENTRY_SIZE));

            // End of directory
            if (raw->name[0] == 0x00) return false;

            // Deleted entry
            if ((uint8_t)raw->name[0] == 0xE5) {
                _entryIndex++;
                continue;
            }

            // Skip LFN and Volume IDs
            if (raw->attr == FAT_ATTR_LFN || (raw->attr & FAT_ATTR_VOLUME_ID)) {
                _entryIndex++;
                continue;
            }

            // Extract 8.3 naming (Keep this, char arrays don't have alignment issues)
            for (int i = 0; i < 8; i++) entry.name[i] = raw->name[i];
            entry.name[8] = '\0';
            for (int i = 0; i < 3; i++) entry.extension[i] = raw->name[8+i];
            entry.extension[3] = '\0';

            entry.attributes = raw->attr;

            // Use a direct offset (28 is the fileSize offset in a 32-byte FAT entry)
            uint8_t* rawEntryBase = _sectorBuffer + (_entryIndex * FAT_DIR_ENTRY_SIZE);
            entry.fileSize = util::readLE32(rawEntryBase + 28);

            uint32_t high = util::readLE16(rawEntryBase + 20);
            uint32_t low  = util::readLE16(rawEntryBase + 26);
            entry.firstCluster = (high << 16) | low;

            uint8_t* sizePtr = (uint8_t*)&raw->fileSize;

            _entryIndex++; 
            return true;
        }

        _entryIndex = 0;
        _sectorIndex++;
        if (_sectorIndex >= _clustertable.bpb().sectorsPerCluster()) {
            _sectorIndex = 0;
            _currentCluster = _clustertable.nextCluster(_currentCluster);
            // The while loop condition will catch the End of Chain
        }
    }

    return false;
}