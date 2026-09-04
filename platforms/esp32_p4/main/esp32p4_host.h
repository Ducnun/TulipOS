#pragma once

#include "hal/ihost.h"
#include "ps2_keyboard.h"
#include "uart_terminal.h"
#include "sd_card_spi.h"
#include "mem_manager.h"

class ESP32P4Host : public IHost {
public:
    ESP32P4Host(SDCardSPI& sdCard, MemoryManager& memory, UARTTerminal& terminal, PS2Keyboard& keyboard) 
        : _sdCard(sdCard), _memory(memory), _terminal(terminal), _keyboard(keyboard)
    {}

    // --- Hardware Discovery ---
    std::vector<StorageDescriptor> getAvailableStorage();

    // --- Platform dependent hardware controllers ---
    IMemoryManager* getMemory(){return &_memory; }
    ITerminal* getTerminal(){ return &_terminal; }
    IKeyboard* getKeyboard(){ return &_keyboard; }

    /**
     * @brief Services any background tasks during busy waiting
     */
    void service();

    /**
     * @brief Executes platform dependent delay
     */
    void delay(uint32_t ms);

    /**
     * @brief Executes platform dependent "millis" (ms since boot)
     */
    uint32_t millis();
    
private:
    // Drives
    SDCardSPI& _sdCard;

    // General devices
    MemoryManager& _memory;
    UARTTerminal& _terminal;
    PS2Keyboard& _keyboard;
};