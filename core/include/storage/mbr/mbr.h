#pragma once

#include "hal/iblock_device.h"
#include "storage/block/partition.h"
#include "storage/mbr/partition_entry.h"
#include "utils/endian.h"

#include <vector>

class MBR {
public:
    bool read(IBlockDevice& device) {
        _partitions.clear();
        alignas(4) uint8_t sector[512]; // Force stack alignment

        if (!device.readBlock(0, sector)) return false;
            
        // Check 0x55AA signature
        if (sector[510] != 0x55 || sector[511] != 0xAA) return false;

        // MBR Partition Table starts at offset 446 (0x1BE)
        // Each entry is 16 bytes
        for (int i = 0; i < 4; i++) {
            uint16_t entryOffset = 446 + (i * 16);
            
            // Offset 4 is the Partition Type byte
            uint8_t type = sector[entryOffset + 4];
            
            if (type != 0) {
                // Use utils to read 32-bit values from unaligned offsets
                uint32_t firstLBA = util::readLE32(&sector[entryOffset + 8]);
                uint32_t sectorCount = util::readLE32(&sector[entryOffset + 12]);
                
                _partitions.emplace_back(firstLBA, sectorCount, type);
            }
        }

        return !_partitions.empty();
    }

    size_t partitionCount() const { return _partitions.size(); }
    const Partition& getPartition(size_t index) const { return _partitions[index]; }

private:
    std::vector<Partition> _partitions;
};