#include "devices/sd_card_spi.h"
#include "Arduino.h"
#include "SPI.h"

SDCardSPI::SDCardSPI(uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t cs) :
    _mosi(mosi), _miso(miso), _sck(sck), _cs(cs), _spiSpeed(SPI_INIT_SPEED), _is_initialized(false) {}

bool SDCardSPI::begin(){
    _is_initialized = false;
    // Initialisation, slow mode
    Serial.printf("Starting SD SPI init\n");
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);
    SPI.begin(_sck, _miso, _mosi, _cs);

    Serial.println("Starting transaction at 100KHz");
    // Start slow transaction
    SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));
    for(int i = 0; i < 10; i++) SPI.transfer(0xFF); // First send at least 74 clock cycles with CS high to boot SD
    SPI.endTransaction();

    // Set in idle state
    uint8_t r1 = sendCommand(SD_SPI_GO_IDLE_STATE, 0, 0x95);
    int check_val = (int)r1;
    if (r1 != SD_SPI_R1_IN_IDLE_STATE) {
        Serial.printf("CMD0 Failed. Response: 0x%02X\n", r1);
        return false;
    }

    // Check voltage (differ per card version e.g. SDHC/SDXC)
    r1 = sendCommand(SD_SPI_SEND_IF_COND, 0x1AA, 0x87);
    if (r1 & SD_SPI_R1_ILLEGAL_COMMAND) {
        Serial.println("Legacy card detected (Ver 1.x)");
        _cardType = SD_CARD_TYPE::SDSC;
    }
    else {
        Serial.println("Ver 2.0+ detected, reading 4 more bytes");
        _cardType = SD_CARD_TYPE::SDHC;
        select();
        for (int i = 0; i < 4; i++) SPI.transfer(0xFF);
        deselect();
    }

    // Initialize card (wait for IDLE bit to clear)
    uint32_t timeout = millis();
    do {
        sendCommand(SD_SPI_APP_CMD, 0, 0xFF);   // Preamble of extended codes
        r1 = sendCommand(SD_SPI_APP_SEND_OP_COND, 0xFF);
    } while ( r1 == SD_SPI_R1_IN_IDLE_STATE && millis() - timeout < 1000 );
    if (millis() - timeout > 1000) Serial.println("SD init timeout reached");

    // Speed up to data rates
    _spiSpeed = SPI_DATA_SPEED;
    Serial.println("SD Card initialized successfully");
    _is_initialized = true;
    return true;
}

bool SDCardSPI::readBlock(uint32_t addr, uint8_t* buf){
    uint32_t byteAddr = addr;
    if (_cardType == SD_CARD_TYPE::SDHC) {
        byteAddr *= 512;  // SDHC uses sector addressing
    }

    // 1. Send the Read Command 
    uint8_t r1 = sendCommand(SD_SPI_READ_SINGLE_BLOCK, byteAddr, 0xFF);
    
    if (r1 != 0x00) {
        Serial.printf("Read Command Failed: 0x%02X\n", r1);
        deselect(); // Ensure we close the transaction on failure
        return false;
    }

    // 2. Wait for the Data Token (0xFE)
    uint32_t timeout = millis();
    uint8_t token = 0xFF;
    while ((token = SPI.transfer(0xFF)) == 0xFF) {
        if (millis() - timeout > 500) {
            Serial.println("Read timeout waiting for 0xFE");
            deselect();
            return false;
        }
    }

    if (token != 0xFE) {
        Serial.printf("Unexpected Data Token: 0x%02X\n", token);
        deselect();
        return false;
    }

    // 3. Read the 512 bytes
    for (uint16_t i = 0; i < 512; i++) {
        buf[i] = SPI.transfer(0xFF);
    }

    // 4. Read the 2-byte CRC (even if ignoring it)
    SPI.transfer(0xFF);
    SPI.transfer(0xFF);

    // 5. Cleanup
    deselect();
    return true;
}

bool SDCardSPI::writeBlock(uint32_t addr, const uint8_t* buf){

}

bool SDCardSPI::isPresent() {
    if (!_is_initialized) return false;

    // If the card has been unplugged, SPI.transfer will usually return 0xFF.
    uint8_t r1 = sendCommand(SD_SPI_SEND_STATUS, 0, 0xFF);
    if (r1 == 0xFF) {
        _is_initialized = false;
        return false;
    }
    return true;
}

uint8_t SDCardSPI::sendCommand(uint8_t cmd, uint32_t arg, uint8_t crc) {
    uint8_t response;
    
    select();
    
    SPI.transfer(0x40 | cmd);
    SPI.transfer(arg >> 24);
    SPI.transfer(arg >> 16);
    SPI.transfer(arg >> 8);
    SPI.transfer(arg);
    SPI.transfer(crc);

    response = waitResponse();
    
    // For many commands, we keep it selected to read more data, 
    // but for simple commands, we deselect here.
    if (cmd != SD_SPI_READ_SINGLE_BLOCK && cmd != SD_SPI_READ_MULTIPLE_BLOCK) deselect(); 
    
    return response;
}

void SDCardSPI::select() {
    // Uses whatever speed we are currently set to (400kHz or 20MHz)
    SPI.beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE0));
    digitalWrite(_cs, LOW);
}

void SDCardSPI::deselect() {
    digitalWrite(_cs, HIGH);
    SPI.transfer(0xFF); // Provide 8 extra clocks for card to release MISO
    SPI.endTransaction();
}

uint8_t SDCardSPI::waitResponse(){
    uint8_t res;
    int timeout = 0;
    while (((res = SPI.transfer(0xFF)) == 0xFF) && timeout < 1000) {
        timeout++;
    }
    return res;
}