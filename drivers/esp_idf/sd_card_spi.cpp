#include "sd_card_spi.h"
#include "utils/sd_spi_commands.h"

#include "driver/gpio.h"
#include "esp_log.h"

#include <cstring>

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

    if (r1 & SD_SPI_R1_ILLEGAL_COMMAND) {

        ESP_LOGI(
            TAG,
            "Legacy card detected (Ver 1.x)"
        );

        _cardType = SD_CARD_TYPE::SDSC;

    } else {

        ESP_LOGI(
            TAG,
            "Ver 2.0+ detected"
        );

        _cardType = SD_CARD_TYPE::SDHC;

        /*
         * CMD8 has four additional response bytes.
         */
        select();

        for (int i = 0; i < 4; ++i) {
            transferByte(0xFF);
        }

        deselect();
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

        sendCommand(
            SD_SPI_APP_CMD,
            0,
            0xFF
        );

        r1 = sendCommand(
            SD_SPI_APP_SEND_OP_COND,
            0xFF,
            0xFF
        );

    } while (
        r1 == SD_SPI_R1_IN_IDLE_STATE &&
        (esp_log_timestamp() - start) < 1000
    );

    if ((esp_log_timestamp() - start) >= 1000) {
        ESP_LOGE(TAG, "SD init timeout reached");
        return false;
    }

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

void SDCardSPI::transfer(const uint8_t* tx, uint8_t* rx, size_t length){
    spi_transaction_t transaction = {};

    transaction.length = length * 8;
    transaction.tx_buffer = tx;
    transaction.rx_buffer = rx;

    transaction.override_freq_hz = _spiSpeed;

    esp_err_t err = spi_device_polling_transmit(_spi, &transaction);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "SPI transfer failed: %s", esp_err_to_name(err));
    }
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

    uint8_t r1 = sendCommand(SD_SPI_READ_SINGLE_BLOCK, addr, 0xFF);
    if (r1 != 0x00) {
        ESP_LOGE(TAG, "CMD17 error, R1=0x%02X", r1);
        return false;
    }

    uint32_t startMs = esp_log_timestamp();
    uint8_t token;
    do {
        token = transferByte(0xFF);
        if ((esp_log_timestamp() - startMs) > 500) {   // 500 ms timeout
            ESP_LOGE(TAG, "Timeout waiting for data token");
            return false;
        }
    } while (token == 0xFF);

    if (token != 0xFE) {
        ESP_LOGE(TAG, "Bad data token 0x%02X", token);
        return false;
    }

    alignas(4) uint8_t tmp[512];   // align to 4‑bytes just to be safe
    memset(tmp, 0xFF, sizeof(tmp));   // TX filler – the card ignores it
    transfer(tmp, tmp, 512);   // tx buffer = dummy 0xFF, rx buffer = tmp
    memcpy(outBuffer, tmp, 512);
    transferByte(0xFF);
    transferByte(0xFF);

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

    if (cmd != SD_SPI_READ_SINGLE_BLOCK && cmd != SD_SPI_READ_MULTIPLE_BLOCK) {
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