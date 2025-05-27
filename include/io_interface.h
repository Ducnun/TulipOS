#pragma once

#include <string>

class IOInterface {
public: 
    /**
     * @brief Destroy the IOInterface object
     */
    virtual ~IOInterface() = default;

    /**
     * @brief Prints to IO
     * @param txt 
     */
    virtual void print(const std::string& txt) = 0;

    /**
     * @brief Reads line from IO until '\n'
     * @return std::string 
     */
    virtual std::string readLine() = 0;

    /**
     * @brief Clears display buffer
     */
    virtual void clearScreen() = 0;

    /**
     * @brief Set the Cursor object
     * @param x 
     * @param y 
     */
    virtual void setCursor(int x, int y) = 0;

    /**
     * @brief Checks if the pause/break key is pressed (ESP32 only). Used for interrupting programs
     * @return true Key press is read and buffered
     * @return false Not pressed
     */
    virtual bool checkBreakRequested() {return false;}

    /**
     * @brief Polls whether an interrupt is needed
     */
    virtual void pollKeyboard(){}
};
