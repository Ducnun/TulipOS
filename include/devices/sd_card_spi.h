#pragma once

#include "storage/block/iblock_device.h"
#include "utils/sd_spi_commands.h"

#define SPI_INIT_SPEED 100000
#define SPI_DATA_SPEED 20000000

enum SD_CARD_TYPE{
    SDSC,
    SDHC,
};

class SDCardSPI : public IBlockDevice{
public:
    SDCardSPI(uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t cs);

    bool begin() override;
    bool readBlock(uint32_t addr, uint8_t* buf) override;
    bool writeBlock(uint32_t addr, const uint8_t* buf) override;
    bool isPresent() override;

private:
    uint8_t _mosi;
    uint8_t _miso;
    uint8_t _sck;
    uint8_t _cs;
    uint32_t _spiSpeed;
    uint8_t _is_initialized;
    SD_CARD_TYPE _cardType;

    // Protocol helpers
    uint8_t sendCommand(uint8_t cmd, uint32_t arg, uint8_t crc = 0x01);
    uint8_t waitResponse();
    void select();
    void deselect();
};