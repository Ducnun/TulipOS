#include "storage/filesystems/fat32/fat32_file.h"
#include <algorithm>
#include <memory.h>

FAT32File::FAT32File(IBlockDevice& device, FAT32ClusterTable& clusters, const FAT32DirectoryEntry& entry)
    : _device(device), _clustertable(clusters),
    _firstCluster(entry.firstCluster), _currentCluster(entry.firstCluster), _fileSize(entry.fileSize),
    _position(0), _sectorInCluster(0), _bufferedSector(0xFFFFFFFF) 
{}

size_t FAT32File::read(uint8_t* buffer, size_t len) {
    size_t bytesRead = 0;

    // Don't read past the end of the file
    if (_position >= _fileSize) return 0;
    if (_position + len > _fileSize) len = _fileSize - _position;

    while (bytesRead < len) {
        // 1. Calculate which sector we need
        uint32_t absSector = _clustertable.firstSector(_currentCluster) + _sectorInCluster;

        // 2. Load sector into internal buffer if not already there
        if (_bufferedSector != absSector) {
            if (!_device.readBlock(absSector, _buffer)) break;
            _bufferedSector = absSector;
        }

        // 3. How many bytes can we take from this specific sector?
        uint32_t offsetInSector = _position % 512;
        uint32_t remainingInSector = 512 - offsetInSector;
        uint32_t toCopy = std::min((uint32_t)(len - bytesRead), remainingInSector);

        memcpy(buffer + bytesRead, _buffer + offsetInSector, toCopy);

        // 4. Update state
        bytesRead += toCopy;
        _position += toCopy;

        // 5. Advance Sector/Cluster pointers if we crossed a boundary
        if (_position % 512 == 0) {
            _sectorInCluster++;
            if (_sectorInCluster >= _clustertable.bpb().sectorsPerCluster()) {
                _sectorInCluster = 0;
                _currentCluster = _clustertable.nextCluster(_currentCluster);
                
                if (_clustertable.isEndOfChain(_currentCluster)) {
                    // This should only happen if the file is smaller thanFileSize says
                    break;
                }
            }
        }
    }

    return bytesRead;
}