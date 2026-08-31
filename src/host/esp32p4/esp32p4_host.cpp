#include "host/esp32p4_host.h"

void ESP32P4Host::begin(){
    
}

void ESP32P4Host::service(){

}

void ESP32P4Host::print(const std::string& txt){

}

std::string ESP32P4Host::readLine(){
    return "";
}

void ESP32P4Host::redrawLine(std::string& line){

}

void ESP32P4Host::clearScreen(){

}

void ESP32P4Host::reset(){

}

void ESP32P4Host::setCursor(int x, int y){

}

bool ESP32P4Host::checkBreakRequested(){
    return false;
}

std::vector<StorageDescriptor> ESP32P4Host::getAvailableStorage(void){
    std::vector<StorageDescriptor> devices;
    // The host knows that on ESP32, we have an SD Card.
    devices.push_back({"SD", &_sdCard});
    return devices;
}

void ESP32P4Host::delay(uint32_t ms){

}

uint32_t ESP32P4Host::millis(){
    return 0;
}