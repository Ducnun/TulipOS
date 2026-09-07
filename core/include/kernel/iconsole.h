#pragma once

#include <string>

class IConsole{
public:
    virtual std::string readLine(const std::string& prompt) = 0;
    virtual void print(const std::string& text) = 0;
    virtual void clearScreen(void) = 0;
    virtual bool checkAndResetBreak(void) = 0;
};