#pragma once

#include "hal/iblock_device.h"
#include "partition.h"


class Volume : public IBlockDevice{
public:
    Volume(IBlockDevice& device, const Partition& partition) : _device(device), _start(partition.startLBA()), _size(partition.sectorCount()) {}
    bool begin() override { return true; }
    bool isPresent() override { return _device.isPresent(); }

    bool readBlock(uint32_t block, uint8_t* buffer) override {
        if(block >= _size) return false;
        return _device.readBlock(_start + block, buffer);
    }

    bool writeBlock(uint32_t block, const uint8_t* buffer) override {
        if(block >= _size) return false;
        return _device.writeBlock(_start + block, buffer);
    }

private:
    IBlockDevice& _device;

    uint32_t _start;
    uint32_t _size;
};