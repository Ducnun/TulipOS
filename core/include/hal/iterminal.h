#pragma once

#include <cstdint>
#include <string>

class ITerminal{
public:
    ~ITerminal() = default;

    virtual bool begin() = 0;

    virtual bool sendChar(char c) = 0;
    virtual bool sendString(const std::string& txt) = 0;
    virtual bool redrawLine(const std::string& line) = 0;
    virtual bool setCursor(uint8_t x, uint8_t y) = 0;
    virtual bool clearScreen() = 0;
    virtual bool setBackground(uint8_t color) = 0;
    virtual bool setColor(uint8_t color) = 0;
    virtual bool backspace() = 0;
    virtual bool newline() = 0;
    virtual bool scroll(int8_t dir, uint8_t lines = 1) = 0;
    virtual bool resync() = 0;
};