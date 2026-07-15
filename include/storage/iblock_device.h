#pragma once

#include <cstdint>

class IBlockDevice{
public:
    virtual ~IBlockDevice() = default;
    static const uint16_t BLOCKSIZE = 512;

    virtual bool begin() = 0;
    virtual bool readBlock(uint32_t blockAddress, uint8_t* buffer) = 0;
    virtual bool writeBlock(uint32_t blockAddress, const uint8_t* buffer) = 0;\
    virtual bool isPresent() = 0;
};