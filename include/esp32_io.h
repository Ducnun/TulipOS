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
                char c = _keyboard.read();

                if (c == '\0') continue;

                if (c == '\n') {
                    Serial.println("");   
                    break;
                }

                if (c == '\b') {
                    if (!line.empty()) {
                        line.pop_back();
                        Serial.print("\b \b");  // Erase last char visually
                    }
                    continue;
                }

                line += c;
                Serial.print(c);
            }
        }
        return line;
    }

    void clearScreen() override {
        Serial.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    }

    void setCursor(int x, int y) override {}

private: 
    PS2Keyboard& _keyboard;
};