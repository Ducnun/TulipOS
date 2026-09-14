#include "kernel/output_service.h"

void OutputService::print(util::StringView txt){
    _terminal.print(txt);
}

void OutputService::print(char c){
    _terminal.sendChar(c);
}

void OutputService::println(util::StringView txt){
    _terminal.print(txt);
    _terminal.newline();
}

void OutputService::clear(){
    _terminal.clearScreen();
}

void OutputService::setColor(uint8_t fg, uint8_t bg){
    _terminal.setColor(fg);
    _terminal.setColor(bg);
}