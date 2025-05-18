#pragma once

#include <Arduino.h>
#include <queue>
#include <unordered_map> 

enum KeyAction {
    None,
    Char,
    Pause,
};

struct KeyEvent {
    KeyAction action;
    char character;
};

class PS2Keyboard {
public:
    PS2Keyboard(uint8_t dataPin, uint8_t clockPin);
    void begin();
    bool available();
    KeyEvent readEvent();

    // DEBUGGING FUNCTION
    void dumpDebugPS2();

private:
    static void IRAM_ATTR handleClockInterrupt(void* arg);
    void onClockFallingEdge();

    uint8_t _dataPin;
    uint8_t _clockPin;
    
    // Start reading inputs after interrupt
    volatile uint8_t _bitCount = 0;
    volatile uint8_t _currentByte = 0;
    volatile bool _receiving = false;
    uint8_t pauseState = 0;

    // Track state of modifiers
    bool shiftHeld = false;
    bool capsLockActive = false;
    bool ctrlHeld = false;
    bool altHeld = false;

    std::queue<uint8_t> _scancodeBuffer;
    portMUX_TYPE _mux = portMUX_INITIALIZER_UNLOCKED;

    char translateScancode(uint8_t code);
    bool handleKeyPressEvent(uint8_t code);
    void handleKeyReleaseEvent(uint8_t code);
    uint8_t readRawScancode();
    void sendByteToKeyboard(uint8_t data);
    void setLEDs(bool caps, bool num, bool scroll);
    bool trackPauseState(uint8_t code);
    bool isPauseSequenceInProgress();
};
