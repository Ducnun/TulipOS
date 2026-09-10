#include "kernel/keyboard_input_service.h"

bool KeyboardInputService::getKeyEvent(KeyEvent& ev){
    return _keyboard.available() ? (ev = _keyboard.readEvent(), true) : false;
}

bool KeyboardInputService::getChar(char& outChar){
    KeyEvent ev;
    if (getKeyEvent(ev) && ev.action == KeyAction::Char){
        outChar = ev.character;
        return true;
    }
    return false;
}

char KeyboardInputService::waitForChar(){
    char c;
    while(true){
        if(getChar(c)) return true;
        _system.yield();
    }
}

void KeyboardInputService::readLine(char* buffer, size_t maxLen){
    _linePos = 0;
    
}