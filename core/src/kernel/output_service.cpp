#include "kernel/output_service.h"

void OutputService::print(util::span<char> txt){
    _terminal.sendString(txt);
}

void OutputService::println(util::span<char> txt){
    _terminal.sendString(txt);
    _terminal.newline();
}

void OutputService::clear(){
    _terminal.clearScreen();
}

void OutputService::setColor(uint8_t fg, uint8_t bg){
    _terminal.setColor(fg);
    _terminal.setColor(bg);
}