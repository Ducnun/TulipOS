#pragma once

#include <iostream>
#include "io_interface.h"

class ConsoleIO : public IOInterface{
public: 
    void print(const std::string& txt) override {
        std::cout << txt;
    }

    std::string readLine() override {
        std::string line;
        std::getline(std::cin, line);
        return line;
    }

    void clearScreen() override {
        std::cout << "\033[2J\033[H";
    }

    void setCursor(int x, int y) override {
        std::cout << "\033[" << y << ";" << x << "H";
    }
};