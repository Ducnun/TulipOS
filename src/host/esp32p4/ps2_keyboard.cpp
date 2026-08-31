#include "devices/ps2_keyboard.h"

#include "driver/gpio.h"
#include "esp_attr.h"

void PS2Keyboard::handleClockInterrupt(void* arg){

}

PS2Keyboard::PS2Keyboard(uint8_t dataPin, uint8_t clockPin){

}

void PS2Keyboard::begin(){
    
}

bool PS2Keyboard::available(){
    return false;
}

KeyEvent PS2Keyboard::readEvent(){
    return {KeyAction::Char, '0'};
}

void PS2Keyboard::onClockFallingEdge(){

}

KeyEvent PS2Keyboard::translateScancode(uint8_t code){
    return {KeyAction::Char, '0'};
}

KeyEvent PS2Keyboard::translateExtendedScancode(uint8_t code){
    return {KeyAction::Char, '0'};
}

bool PS2Keyboard::handleKeyPressEvent(uint8_t code){
    return false;
}

void PS2Keyboard::handleKeyReleaseEvent(uint8_t code, bool extended){

}

uint8_t PS2Keyboard::readRawScancode(){
    return 0;
}

void PS2Keyboard::sendByteToKeyboard(uint8_t data){

}

void PS2Keyboard::setLEDs(bool caps, bool num, bool scroll){

}

bool PS2Keyboard::trackPauseState(uint8_t code){
    return false;
}

bool PS2Keyboard::isPauseSequenceInProgress(){
    return false;
}

void PS2Keyboard::debugPrintState(const char* context, uint8_t code){

}
