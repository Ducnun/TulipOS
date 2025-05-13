#pragma once

#include <Arduino.h>
#include <queue>
#include <unordered_map> 


class PS2Keyboard {
public:
    PS2Keyboard(uint8_t dataPin, uint8_t clockPin);
    void begin();
    bool available();
    char read();

    // DEBUGGING FUNCTION
    void dumpDebugPS2();

private:
    static void IRAM_ATTR handleClockInterrupt(void* arg);
    void onClockFallingEdge();

    uint8_t _dataPin;
    uint8_t _clockPin;

    volatile uint8_t _bitCount = 0;
    volatile uint8_t _currentByte = 0;
    volatile bool _receiving = false;

    std::queue<uint8_t> _scancodeBuffer;
    portMUX_TYPE _mux = portMUX_INITIALIZER_UNLOCKED;

    // DEBUG BUFFER
    volatile uint8_t debugCodes[32];
    volatile int debugWriteIndex = 0;
    volatile int debugReadIndex = 0;

    char translateScancode(uint8_t code);
};

