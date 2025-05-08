#pragma once

#include <string>

class IOInterface {
public: 
    virtual ~IOInterface() = default;
    virtual void print(const std::string& txt) = 0;
    virtual std::string readLine() = 0;
    virtual void clearScreen() = 0;
    virtual void setCursor(int x, int y) = 0;
};
