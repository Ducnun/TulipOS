#pragma once

#include "hal/ikeyboard.h"  // Hardware interface

enum class InputMode {
    Immediate,  // Non-blocking (for games, real-time input)
    Line,       // Blocking, buffers until Enter (for BASIC/REPL)
};

class IInputService {
public:
    virtual ~IInputService() = default;

    // --- Low-level: Direct access to keyboard events ---
    virtual bool getKeyEvent(KeyEvent& event) = 0;  // Non-blocking

    // --- Mid-level: Character-based input ---
    virtual bool getChar(char& outChar) = 0;         // Non-blocking
    virtual char waitForChar() = 0;                 // Blocking

    // --- High-level: Line input (like BASIC's INPUT) ---
    virtual void readLine(char* buffer, size_t maxLen) = 0;

    // --- Mode control (for retro vs. modern behavior) ---
    virtual void setInputMode(InputMode mode) = 0;
    virtual InputMode getInputMode() const = 0;
};