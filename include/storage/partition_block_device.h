#pragma once

#include <stdint.h>
#include "storage/iblock_device.h"

class PartitionBlockDevice : public IBlockDevice
{
public:
    PartitionBlockDevice(IBlockDevice& disk, uint32_t firstLBA);

    bool readBlock(uint32_t block, uint8_t* buf) override
    {
        return _disk.readBlock(firstLBA + block, buf);
    }

private:
    IBlockDevice& _disk;
    uint32_t firstLBA;
};