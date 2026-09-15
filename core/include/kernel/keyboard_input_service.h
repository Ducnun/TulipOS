#pragma once

#include "kernel/api/iinput_service.h"
#include "kernel/api/itask_control.h"

class KeyboardInputService : public IInputService{
public:
    KeyboardInputService(IKeyboard& keyboard, ITaskControl& taskControl) 
        : _keyboard(keyboard), _taskControl(taskControl)
    {} ;

    // Start keyboard driver (configuring perhipherals, attaching interrupts etc.)
    void begin() { _keyboard.begin(); };

    // IInputService implementation
    bool getKeyEvent(KeyEvent& ev); // Reads raw key event
    void waitForKeyEvent(KeyEvent& ev); // Reads raw key event
    bool getChar(char& outChar);    // Non blocking 
    char waitForChar();             // Blocking
    void readLine(char* buffer, size_t maxLen);
    void setInputMode(InputMode mode) { _mode = mode; };
    InputMode getInputMode() const { return _mode; };
    
private:
    IKeyboard& _keyboard;
    ITaskControl& _taskControl;
    InputMode _mode = InputMode::Immediate;
    char _linebuffer[256];
    size_t _linePos;
};