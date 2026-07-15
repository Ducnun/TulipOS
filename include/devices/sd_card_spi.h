#pragma once

#include "storage/iblock_device.h"

class SDCardSPI : public IBlockDevice{
public:
    SDCardSPI(uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t cs);

    bool begin() override;
    bool readBlock(uint32_t adr, uint8_t* buf) override;
    bool writeBlock(uint32_t adr, const uint8_t* buf) override;
    bool isPresent() override;

private:
    uint8_t _mosi;
    uint8_t _miso;
    uint8_t _sck;
    uint8_t _cs;
};