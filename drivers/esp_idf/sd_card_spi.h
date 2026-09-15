#pragma once

#include <cstdint>

#include "driver/spi_master.h"
#include "hal/iblock_device.h"

class SDCardSPI : public IBlockDevice {
public:
    SDCardSPI(spi_host_device_t host, int mosi, int miso, int sck, int cs);

    bool begin() override;
    bool readBlock(uint32_t blockAddress, uint8_t* buffer) override;
    bool writeBlock(uint32_t blockAddress, const uint8_t* buffer) override{
        return false;   // Temporary stub
    }
    bool isPresent();

private:
    static constexpr uint32_t SPI_INIT_SPEED = 400'000;
    static constexpr uint32_t SPI_DATA_SPEED = 20'000'000;

    spi_host_device_t _host;

    int _mosi;
    int _miso;
    int _sck;
    int _cs;

    spi_device_handle_t _spi;

    uint32_t _spiSpeed;
    bool _isInitialized;

    enum class SD_CARD_TYPE {
        SDSC,
        SDHC
    };

    SD_CARD_TYPE _cardType;

    uint8_t transferByte(uint8_t value);
    void transfer(const uint8_t* tx, uint8_t* rx, size_t length);
    uint8_t sendCommand(uint8_t cmd, uint32_t arg, uint8_t crc);
    uint8_t waitResponse();
    void select();
    void deselect();
};