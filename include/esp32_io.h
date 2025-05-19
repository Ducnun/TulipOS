#pragma once

#include "io_interface.h"
#include "ps2_keyboard.h"

class ESP32IO : public IOInterface{
public: 
    ESP32IO(PS2Keyboard& keyboard): _keyboard(keyboard) {}
    void begin() {
        Serial.begin(115200);
        _keyboard.begin();
    } 

    void print(const std::string &txt) override {
        Serial.print(txt.c_str());
    }

    std::string readLine() override {
        std::string line;
        while (true) {
            if (_keyboard.available()) {
                KeyEvent key = _keyboard.readEvent();

                if (key.action == KeyAction::None) continue;

                if (key.action == KeyAction::Char && key.character == '\n') {
                    Serial.println("");   
                    break;
                }

                if (key.action == KeyAction::Char && key.character == '\b') {
                    if (!line.empty()) {
                        line.pop_back();
                        Serial.print("\b \b");  // Erase last char visually
                    }
                    continue;
                }

                if (key.action == KeyAction::Pause){
                    _breakRequested = true;
                    return "";
                }

                if (key.action == KeyAction::Char){
                    line += key.character;
                    Serial.print(key.character);
                }
            }
        }
        return line;
    }

    void clearScreen() override {
        Serial.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    }

    void setCursor(int x, int y) override {}

    bool checkBreakRequested() override {
        if (_breakRequested){
            _breakRequested = false;
            return true;
        }
        return false;
    }

    void pollKeyboard(){
        if (_keyboard.available()){
            KeyEvent key = _keyboard.readEvent();
            if (key.action == KeyAction::Pause){
                _breakRequested = true;
            }
        }
    }

private: 
    PS2Keyboard& _keyboard;
    bool _breakRequested = false;
};