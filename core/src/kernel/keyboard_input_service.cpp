#include "kernel/keyboard_input_service.h"

bool KeyboardInputService::getKeyEvent(KeyEvent& ev){
    return _keyboard.available() ? (ev = _keyboard.readEvent(), true) : false;
}

void KeyboardInputService::waitForKeyEvent(KeyEvent& ev){
    while (true){
        _taskControl.yield();
        ev = _keyboard.readEvent();
        if (ev.action != KeyAction::None) return;
    }
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
        if(getChar(c)) return c;
        _taskControl.yield();
    }
}

void KeyboardInputService::readLine(char* buffer, size_t maxLen){
    _linePos = 0;
    while (_linePos < maxLen - 1){
        char c = waitForChar();
        if (c == '\n' || c == '\r'){
            buffer[_linePos] = '\0';    // Make null termination from new line/carriage return
            return;
        }
        else if (c == '\b'){
            if (_linePos > 0) _linePos--;
        }
        else{
            buffer[_linePos++] = c;
        }
    }
    buffer[_linePos] = '\0';    // Null terminate end of string
}