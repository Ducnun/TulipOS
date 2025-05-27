#pragma once

#include <iostream>
#include "io_interface.h"

class ConsoleIO : public IOInterface{
public: 
    /**
     * @brief Prints to a terminal/console
     * @param txt Text to print
     */
    void print(const std::string& txt) override {
        std::cout << txt;
    }

    /**
     * @brief Reads a line from the terminal/console up until a '\n'
     * @return std::string The raw input string
     */
    std::string readLine() override {
        std::string line;
        std::getline(std::cin, line);
        return line;
    }

    /**
     * @brief Sends the clear screen command to the console
     */
    void clearScreen() override {
        std::cout << "\033[2J\033[H";
    }

    /**
     * @brief Set the Cursor object
     * @param x 
     * @param y 
     */
    void setCursor(int x, int y) override {
        std::cout << "\033[" << y << ";" << x << "H";
    }
};