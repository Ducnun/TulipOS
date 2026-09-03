#include "uart_terminal.h"


bool UARTTerminal::begin(){
    return true;
}

bool UARTTerminal::sendChar(char c){
    return true;
}

bool UARTTerminal::sendString(const std::string& txt){
    return true;
}

bool UARTTerminal::redrawLine(const std::string& line){
    return true;
}

bool UARTTerminal::setCursor(uint8_t x, uint8_t y){
    return true;
}

bool UARTTerminal::clearScreen(){
    return true;
}

bool UARTTerminal::setBackground(uint8_t color){
    return true;
}

bool UARTTerminal::backspace(){
    return true;
}

bool UARTTerminal::newline(){
    return true;
}

bool UARTTerminal::scroll(int8_t dir, uint8_t lines){
    return true;
}

bool UARTTerminal::resync(){
    return true;
}

bool UARTTerminal::sendMessage(STREAM_CMD cmd, uint16_t length, const uint8_t* data){
    return true;
}

bool UARTTerminal::sendPacket(STREAM_CMD cmd, uint8_t length, const uint8_t* data){
    return true;
}
