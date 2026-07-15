#include "devices/sd_card_spi.h"

SDCardSPI::SDCardSPI(uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t cs) :
    _mosi(mosi), _miso(miso), _sck(sck), _cs(cs) {

}

bool SDCardSPI::begin(){
    
}