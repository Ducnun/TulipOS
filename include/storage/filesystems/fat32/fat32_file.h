#pragma once

#include "../vfs/ifile.h"
#include "storage/block/iblock_device.h"
#include "fat32_clustertable.h"
#include "fat32_directory.h" // For the entry struct

class FAT32File : public IFile {
public:
    FAT32File(IBlockDevice& device, FAT32ClusterTable& clusters, const FAT32DirectoryEntry& entry);

    // Reads 'len' bytes into 'buffer'. Returns actual bytes read.
    size_t read(uint8_t* buffer, size_t len);

    // Seek to a specific byte in the file
    bool seek(uint32_t position);

    void close(void);

    uint32_t size() const { return _fileSize; }
    uint32_t position() const { return _position; }
    bool eof() const { return _position >= _fileSize; }

private:
    IBlockDevice& _device;
    FAT32ClusterTable& _clustertable;

    uint32_t _firstCluster;
    uint32_t _currentCluster;
    uint32_t _fileSize;
    
    uint32_t _position;         // Current byte offset in file
    uint32_t _sectorInCluster;  // Which sector (0-7) of the current cluster
    
    alignas(4) uint8_t _buffer[512];
    uint32_t _bufferedSector;   // Which absolute sector is in _buffer
};