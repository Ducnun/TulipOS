#pragma once

#include "Arduino.h"
#include "io_interface.h"
#include "ps2_keyboard.h"
#include "uart_terminal.h"

class ESP32IO : public IOInterface{
public: 
    /**
     * @brief Construct a new ESP32IO object
     * @param PS2Keyboard
     */
    ESP32IO(PS2Keyboard& keyboard, UARTTerminal& terminal): _keyboard(keyboard), _terminal(terminal){}

    /**
     * @brief Starts the IO class, reading out inputs and setting up outputs
     */
    void begin();

    /**
     * @brief Overrides the standard io_interface print statement to output to the terminal/monitor
     * @param txt 
     */
    void print(const std::string& txt);

    /**
     * @brief Reads the inputs from the provided keyboard
     * @return std::string Returns the raw input
     */
    std::string readLine();

    /**
     * @brief Redraws the currently buffered line (e.g. when moving through history or moving cursor)
     * @param line Line to redraw
     */
    void redrawLine(std::string& line);

    /**
     * @brief Clears the buffer/line
     */
    void clearScreen();

    /**
     * @brief Resets any states of the IO
     */
    void reset();

    /**
     * @brief Set the Cursor object
     * @param x 
     * @param y 
     */
    void setCursor(int x, int y);

    /**
     * @brief Overrides io_interface Check if break input is requested from Keyboard
     * @return true Break is pressed
     * @return false Break is not pressed
     */
    bool checkBreakRequested();

    /**
     * @brief Manual polling (needed for checkBreakRequested)
     */
    void pollKeyboard();

private: 
    PS2Keyboard& _keyboard; ///< Keyboard object used for inputs
    UARTTerminal& _terminal;
    bool _breakRequested = false;   ///< Buffers the break key
    std::vector<std::string> _history;  ///< Stores command history
    int _historyIndex = -1; ///< Tracks currently selected index in history
    std::string _currentLine;   ///< Currently selected/typed and drawn line
};