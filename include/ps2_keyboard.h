#pragma once

#include <Arduino.h>
#include <queue>
#include <unordered_map> 

enum KeyAction {
    None,
    Char,
    ArrowLeft,
    ArrowRight,
    ArrowUp,
    ArrowDown,
    Pause,
};

struct KeyEvent {
    KeyAction action;
    char character;
};

enum ScancodeState {
    Normal,
    ExpectExtended,
    ExpectBreak,
    ExpectExtendedBreak,
};

class PS2Keyboard {
public:
    PS2Keyboard(uint8_t dataPin, uint8_t clockPin);
    void begin();
    bool available();
    KeyEvent readEvent();

private:
    static void IRAM_ATTR handleClockInterrupt(void* arg);
    void onClockFallingEdge();

    uint8_t _dataPin;
    uint8_t _clockPin;
    
    // Start reading inputs after interrupt
    volatile uint8_t _bitCount = 0;
    volatile uint8_t _currentByte = 0;
    volatile uint8_t _parity = 0;
    volatile bool _receiving = false;
    ScancodeState _scancodeState;
    uint8_t pauseState = 0;

    // Track state of modifiers
    bool _capsLockActive = false;
    bool _numLockActive = true;
    bool _shiftHeld = false;
    bool _ctrlHeld = false;
    bool _altHeld = false;
    bool _extendedCodeNext = false;
    bool _breakCodeNext = false;

    static const size_t BufferSize = 32;
    volatile uint8_t _scancodeBuffer[BufferSize];
    volatile size_t _head = 0;
    volatile size_t _tail = 0;
    portMUX_TYPE _mux = portMUX_INITIALIZER_UNLOCKED;

    KeyEvent translateScancode(uint8_t code);
    KeyEvent translateExtendedScancode(uint8_t code);
    bool handleKeyPressEvent(uint8_t code);
    void handleKeyReleaseEvent(uint8_t code, bool extended);
    uint8_t readRawScancode();
    void sendByteToKeyboard(uint8_t data);
    void setLEDs(bool caps, bool num, bool scroll);
    bool trackPauseState(uint8_t code);
    bool isPauseSequenceInProgress();
};
