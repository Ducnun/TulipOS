#include "sd_card_spi.h"
#include "utils/sd_spi_commands.h"

#include "driver/gpio.h"
#include "esp_log.h"

#include <cstring>
#include <inttypes.h>

static const char* TAG = "SDCardSPI";

SDCardSPI::SDCardSPI(spi_host_device_t host, int mosi, int miso, int sck, int cs) 
    : _host(host), _mosi(mosi), _miso(miso), _sck(sck), _cs(cs), _spi(nullptr), _spiSpeed(SPI_INIT_SPEED), _isInitialized(false), _cardType(SD_CARD_TYPE::SDSC)
{}

bool SDCardSPI::begin()
{
    _isInitialized = false;

    ESP_LOGI(TAG, "Starting SD SPI init");

    // Configure CS manually.
    gpio_config_t csConfig = {};

    csConfig.pin_bit_mask = 1ULL << _cs;
    csConfig.mode = GPIO_MODE_OUTPUT;
    csConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    csConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    csConfig.intr_type = GPIO_INTR_DISABLE;

    esp_err_t err = gpio_config(&csConfig);

    if (err != ESP_OK) {
        ESP_LOGE(
            TAG,
            "Failed to configure CS: %s",
            esp_err_to_name(err)
        );
        return false;
    }

    gpio_set_level(
        static_cast<gpio_num_t>(_cs),
        1
    );

    // Configure SPI bus.
    spi_bus_config_t busConfig = {};

    busConfig.mosi_io_num = _mosi;
    busConfig.miso_io_num = _miso;
    busConfig.sclk_io_num = _sck;
    busConfig.quadwp_io_num = -1;
    busConfig.quadhd_io_num = -1;

    err = spi_bus_initialize(
        _host,
        &busConfig,
        SPI_DMA_CH_AUTO
    );

    if (err != ESP_OK) {
        ESP_LOGE(
            TAG,
            "SPI bus initialization failed: %s",
            esp_err_to_name(err)
        );
        return false;
    }

    // Configure SD SPI device.
    spi_device_interface_config_t deviceConfig = {};

    deviceConfig.clock_speed_hz = SPI_DATA_SPEED;
    deviceConfig.mode = 0;

    // CS is handled manually by select()/deselect().
    deviceConfig.spics_io_num = -1;

    deviceConfig.queue_size = 1;

    err = spi_bus_add_device(
        _host,
        &deviceConfig,
        &_spi
    );

    if (err != ESP_OK) {
        ESP_LOGE(
            TAG,
            "Failed to add SPI device: %s",
            esp_err_to_name(err)
        );

        spi_bus_free(_host);
        return false;
    }

    /*
     * SD initialization must start slowly.
     */
    _spiSpeed = SPI_INIT_SPEED;

    ESP_LOGI(TAG, "Starting transaction at 400KHz");

    /*
     * CS must remain HIGH while providing at least
     * 74 clock cycles to the card.
     *
     * 10 bytes = 80 clock cycles.
     */
    gpio_set_level(
        static_cast<gpio_num_t>(_cs),
        1
    );

    for (int i = 0; i < 10; ++i) {
        transferByte(0xFF);
    }

    /*
     * CMD0 - GO_IDLE_STATE
     */
    uint8_t r1 = sendCommand(
        SD_SPI_GO_IDLE_STATE,
        0,
        0x95
    );

    if (r1 != SD_SPI_R1_IN_IDLE_STATE) {
        ESP_LOGE(
            TAG,
            "CMD0 failed. Response: 0x%02X",
            r1
        );
        return false;
    }

    /*
     * CMD8 - SEND_IF_COND
     */
    r1 = sendCommand(
        SD_SPI_SEND_IF_COND,
        0x1AA,
        0x87
    );

    bool supportsV2 = false;
    if (r1 == (SD_SPI_R1_IN_IDLE_STATE | SD_SPI_R1_ILLEGAL_COMMAND)) {
        deselect();

        ESP_LOGI(
            TAG,
            "Legacy card detected (Ver 1.x)"
        );

        _cardType = SD_CARD_TYPE::SDSC;

    } else {
        if (r1 != SD_SPI_R1_IN_IDLE_STATE) {
            ESP_LOGE(TAG, "CMD8 failed. Response: 0x%02X", r1);
            deselect();
            return false;
        }

        ESP_LOGI(
            TAG,
            "SD protocol version 2 or later detected"
        );

        supportsV2 = true;

        uint8_t r7[4];
        for (uint8_t& byte : r7) {
            byte = transferByte(0xFF);
        }
        deselect();

        if ((r7[2] & 0x0F) != 0x01 || r7[3] != 0xAA) {
            ESP_LOGE(TAG, "Invalid CMD8 R7 response: %02X %02X %02X %02X",
                     r7[0], r7[1], r7[2], r7[3]);
            return false;
        }
    }

    /*
     * Initialize card:
     *
     * CMD55
     * ACMD41
     *
     * Repeat until the card leaves idle state.
     */
    uint32_t start = esp_log_timestamp();

    do {

        uint8_t appResponse = sendCommand(
            SD_SPI_APP_CMD,
            0,
            0xFF
        );
        if (appResponse != SD_SPI_R1_IN_IDLE_STATE && appResponse != 0) {
            ESP_LOGE(TAG, "CMD55 failed. Response: 0x%02X", appResponse);
            return false;
        }

        r1 = sendCommand(
            SD_SPI_APP_SEND_OP_COND,
            supportsV2 ? SD_SPI_HCS : 0,
            0xFF
        );

        if (r1 != SD_SPI_R1_IN_IDLE_STATE && r1 != 0) {
            ESP_LOGE(TAG, "ACMD41 failed. Response: 0x%02X", r1);
            return false;
        }
    } while (r1 == SD_SPI_R1_IN_IDLE_STATE &&
             (esp_log_timestamp() - start) < 1000);

    if (r1 != 0) {
        ESP_LOGE(TAG, "SD init timeout reached");
        return false;
    }

    // CMD58 returns the OCR. CCS (bit 30) indicates block-addressed SDHC/SDXC.
    r1 = sendCommand(SD_SPI_READ_OCR, 0, 0xFF);
    if (r1 != 0x00) {
        ESP_LOGE(TAG, "CMD58 failed. Response: 0x%02X", r1);
        deselect();
        return false;
    }

    uint32_t ocr = 0;
    for (int i = 0; i < 4; ++i) {
        ocr = (ocr << 8) | transferByte(0xFF);
    }
    deselect();

    if ((ocr & (1UL << 31)) == 0) {
        ESP_LOGE(TAG, "CMD58 returned OCR without power-up complete: 0x%08" PRIX32,
                 ocr);
        return false;
    }

    _cardType = (ocr & (1UL << 30))
        ? SD_CARD_TYPE::SDHC
        : SD_CARD_TYPE::SDSC;

    ESP_LOGI(TAG, "Card addressing: %s (OCR=0x%08" PRIX32 ")",
             _cardType == SD_CARD_TYPE::SDHC ? "block" : "byte", ocr);

    /*
     * Card is initialized.
     * Increase SPI speed for normal operation.
     */
    _spiSpeed = SPI_DATA_SPEED;

    ESP_LOGI(
        TAG,
        "SD Card initialized successfully"
    );

    _isInitialized = true;

    return true;
}

uint8_t SDCardSPI::transferByte(uint8_t value){
    uint8_t rx = 0;

    spi_transaction_t transaction = {};

    transaction.length = 8;
    transaction.tx_buffer = &value;
    transaction.rx_buffer = &rx;

    transaction.override_freq_hz = _spiSpeed;

    esp_err_t err = spi_device_polling_transmit(_spi, &transaction);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "SPI transfer failed: %s", esp_err_to_name(err));
        return 0xFF;
    }

    return rx;
}

bool SDCardSPI::transfer(const uint8_t* tx, uint8_t* rx, size_t length){
    spi_transaction_t transaction = {};

    transaction.length = length * 8;
    transaction.tx_buffer = tx;
    transaction.rx_buffer = rx;

    transaction.override_freq_hz = _spiSpeed;

    esp_err_t err = spi_device_polling_transmit(_spi, &transaction);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "SPI transfer failed: %s", esp_err_to_name(err));
        return false;
    }

    return true;
}

bool SDCardSPI::readBlock(uint32_t blockAddress, uint8_t *outBuffer){
    if (!_isInitialized) {
        ESP_LOGE(TAG, "readBlock called before init");
        return false;
    }

    uint32_t addr = blockAddress;
    if (_cardType == SD_CARD_TYPE::SDSC) {
        addr *= 512;                     // SDSC uses byte addressing
    }                                   // SDHC/SDXC already uses 512‑byte units

    ESP_LOGI(TAG, "CMD17 LBA=%" PRIu32 " argument=0x%08" PRIX32,
             blockAddress, addr);
    uint8_t r1 = sendCommand(SD_SPI_READ_SINGLE_BLOCK, addr, 0xFF);
    if (r1 != 0x00) {
        ESP_LOGE(TAG, "CMD17 error, R1=0x%02X", r1);
        deselect();
        return false;
    }

    uint32_t startMs = esp_log_timestamp();
    uint8_t token;
    do {
        token = transferByte(0xFF);
        if ((esp_log_timestamp() - startMs) > 500) {   // 500 ms timeout
            ESP_LOGE(TAG, "Timeout waiting for data token");
            deselect();
            return false;
        }
    } while (token == 0xFF);

    if (token != 0xFE) {
        ESP_LOGE(TAG, "Bad data token 0x%02X", token);
        deselect();
        return false;
    }

    alignas(4) uint8_t txBuffer[512];
    alignas(4) uint8_t rxBuffer[512];
    memset(txBuffer, 0xFF, sizeof(txBuffer));
    if (!transfer(txBuffer, rxBuffer, sizeof(rxBuffer))) {
        deselect();
        return false;
    }
    memcpy(outBuffer, rxBuffer, sizeof(rxBuffer));
    transferByte(0xFF);
    transferByte(0xFF);
    deselect();

    return true;
}

uint8_t SDCardSPI::sendCommand(uint8_t cmd, uint32_t arg, uint8_t crc){
    select();
    ESP_LOGI(TAG, "CMD%u CS=%d", cmd, gpio_get_level(static_cast<gpio_num_t>(_cs)));
    
    uint8_t b;
    b = transferByte(0x40 | cmd);
    ESP_LOGI(TAG, "TX command: 0x%02X -> RX 0x%02X", 0x40 | cmd, b);

    b = transferByte(arg >> 24);
    b = transferByte(arg >> 16);
    b = transferByte(arg >> 8);
    b = transferByte(arg);
    b = transferByte(crc);

    uint8_t response = waitResponse();
    ESP_LOGI(TAG, "CMD%u response: 0x%02X", cmd, response);

    if (cmd != SD_SPI_SEND_IF_COND &&
        cmd != SD_SPI_READ_SINGLE_BLOCK &&
        cmd != SD_SPI_READ_MULTIPLE_BLOCK &&
        cmd != SD_SPI_READ_OCR) {
        deselect();
    }

    return response;
}

void SDCardSPI::select(){
    gpio_set_level(static_cast<gpio_num_t>(_cs), 0);
}

void SDCardSPI::deselect(){
    gpio_set_level(static_cast<gpio_num_t>(_cs), 1);

    // Give the SD card 8 additional clocks.
    transferByte(0xFF);
}

uint8_t SDCardSPI::waitResponse(){
    uint8_t response;

    int timeout = 0;

    do {
        response = transferByte(0xFF);
        timeout++;
    }
    while (response == 0xFF && timeout < 1000);

    return response;
}

bool SDCardSPI::isPresent(){
    if (!_isInitialized) {
        return false;
    }

    uint8_t r1 = sendCommand(SD_SPI_SEND_STATUS, 0, 0xFF);

    if (r1 == 0xFF) {
        _isInitialized = false;
        return false;
    }

    return true;
}