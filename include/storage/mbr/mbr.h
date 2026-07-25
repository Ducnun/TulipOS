#pragma once

#include "storage/block/iblock_device.h"
#include "storage/block/partition.h"
#include "storage/mbr/partition_entry.h"

#include <vector>


class MBR{
public:
    bool read(IBlockDevice& device){
        uint8_t sector[512];

        if(!device.readBlock(0, sector))
            return false;
            
        // Check 0x55AA signature
        if(sector[510] != 0x55 || sector[511] != 0xAA) return false;
        auto entries = reinterpret_cast<PartitionEntry*>(&sector[0x1BE]);

        for(int i = 0; i < 4; i++){
            if(entries[i].type != 0){
                _partitions.emplace_back(entries[i].firstLBA, entries[i].sectorCount, entries[i].type);
            }
        }

        return true;
    }

    const Partition& partition(size_t index){ return _partitions[index]; }

private:
    std::vector<Partition> _partitions;
};