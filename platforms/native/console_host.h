#pragma once

#include <chrono>
#include <thread>
#include <iostream>
#include "ihost.h"

class ConsoleHost : public IHost{
public: 
    ConsoleHost(){
        // Capture start point in steady clock, used as reference point for millis
        _start = std::chrono::steady_clock::now();
    }

    void service(){
        // Nothing needed, OS handles everything already
    }

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

    /**
     * @brief Clears terminal, no other things to reset for native builds
     */
    void reset() override {
        clearScreen();
    }

    void delay(uint32_t ms) override { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }

    uint32_t millis() override {
        auto now = std::chrono::steady_clock::now();

        return std::chrono::duration_cast<std::chrono::milliseconds>(
            now - _start
        ).count();
    }

private:
    std::chrono::steady_clock::time_point _start;
};