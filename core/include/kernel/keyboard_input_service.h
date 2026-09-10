#pragma once

#include "kernel/api/iinput_service.h"
#include "kernel/api/isystem_service.h"

class KeyboardInputService : public IInputService{
public:
    KeyboardInputService(IKeyboard& keyboard, ISystemService& system) 
        : _keyboard(keyboard), _system(system)
    {} ;

    // Start keyboard driver (configuring perhipherals, attaching interrupts etc.)
    void begin() { _keyboard.begin(); };

    // IInputService implementation
    bool getKeyEvent(KeyEvent& ev); // Reads raw key event
    bool getChar(char& outChar);    // Non blocking 
    char waitForChar();             // Blocking
    void readLine(char* buffer, size_t maxLen);
    void setInputMode(InputMode mode) { _mode = mode; };
    InputMode getInputMode() const { return _mode; };
    
private:
    IKeyboard& _keyboard;
    ISystemService& _system;
    InputMode _mode = InputMode::Immediate;
    char _linebuffer[256];
    size_t _linePos;
};