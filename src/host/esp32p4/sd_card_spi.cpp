#include "devices/sd_card_spi.h"

SDCardSPI::SDCardSPI(uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t cs){

}

bool SDCardSPI::begin() {
    return true;
}

bool SDCardSPI::readBlock(uint32_t addr, uint8_t* buf) {
    return true;
}

bool SDCardSPI::writeBlock(uint32_t addr, const uint8_t* buf) {
    return true;
}

bool SDCardSPI::isPresent() {
    return true;
}

uint8_t SDCardSPI::sendCommand(uint8_t cmd, uint32_t arg, uint8_t crc){
    return 0;
}

uint8_t SDCardSPI::waitResponse(){
    return 0;
}

void SDCardSPI::select(){

}

void SDCardSPI::deselect(){

}
