#pragma once

#include "hal/ihost.h"
#include "ps2_keyboard.h"
#include "uart_terminal.h"
#include "sd_card_spi.h"
#include "mem_manager.h"

class ESP32P4Host : public IHost {
public:
    ESP32P4Host(PS2Keyboard& kb, UARTTerminal& terminal, SDCardSPI& sdCard, MemoryManager& memory) 
        : _kb(kb), _terminal(terminal), _sdCard(sdCard), _memory(memory) 
    {}

        /**
     * @brief Starts the IO class, reading out inputs and setting up outputs
     */
    void begin();

    // --- Hardware Discovery ---
    std::vector<StorageDescriptor> getAvailableStorage();
    IKeyboard& keyboard() { return _kb; };
    ITerminal& terminal() { return _terminal; };

    // --- Platform dependent Services ---
    IMemoryManager& memory() {return _memory; };

    /**
     * @brief Checks up on hardware modules/devices
     */
    void service();
    void debugLog(const char* message) override;

    void delay(uint32_t ms);
    uint32_t millis();
    
private:
    PS2Keyboard& _kb;
    UARTTerminal& _terminal;
    SDCardSPI& _sdCard;
    MemoryManager& _memory;
    
    bool _sdCardInit = false;       ///< Tracks the initialization state of the SD card
    bool _breakRequested = false;   ///< Buffers the break key
    std::vector<std::string> _history;  ///< Stores command history
    int _historyIndex = -1; ///< Tracks currently selected index in history
    std::string _currentLine;   ///< Currently selected/typed and drawn line
};