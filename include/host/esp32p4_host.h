#pragma once

#include "host/ihost.h"
#include "devices/ps2_keyboard.h"
#include "devices/uart_terminal.h"
#include "devices/sd_card_spi.h"

class ESP32P4Host : public IHost {
public:
    ESP32P4Host(PS2Keyboard kb, UARTTerminal terminal, SDCardSPI sdCard);

        /**
     * @brief Starts the IO class, reading out inputs and setting up outputs
     */
    void begin();

    /**
     * @brief Checks up on hardware modules/devices
     */
    void service();

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

    // --- Hardware Discovery ---
    std::vector<StorageDescriptor> getAvailableStorage();

    void delay(uint32_t ms);
    uint32_t millis();
    
private:
    PS2Keyboard& _kb;
    UARTTerminal& _terminal;
    SDCardSPI& _sdCard;

    bool _breakRequested = false;   ///< Buffers the break key
    std::vector<std::string> _history;  ///< Stores command history
    int _historyIndex = -1; ///< Tracks currently selected index in history
    std::string _currentLine;   ///< Currently selected/typed and drawn line
};