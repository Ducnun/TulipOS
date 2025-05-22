#pragma once

#include "io_interface.h"
#include "ps2_keyboard.h"

class ESP32IO : public IOInterface{
public: 
    ESP32IO(PS2Keyboard& keyboard): _keyboard(keyboard) {}
    void begin();
    void print(const std::string& txt);
    std::string readLine();
    void redrawLine(std::string& line);
    void clearScreen();
    void setCursor(int x, int y);
    bool checkBreakRequested();
    void pollKeyboard();

private: 
    PS2Keyboard& _keyboard;
    bool _breakRequested = false;
    std::vector<std::string> _history;
    int _historyIndex = -1;
    std::string _currentLine;
};