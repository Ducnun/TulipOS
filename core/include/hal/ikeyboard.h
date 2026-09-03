#pragma once

#include <cstdint>

/**
 * @brief Enum that holds the different kinds of actions from the keyboard
 */
enum KeyAction {
    None,
    Char,
    ArrowLeft,
    ArrowRight,
    ArrowUp,
    ArrowDown,
    Pause,
};

/**
 * @brief Struct that holds an action and if applicable a value for that action
 */
struct KeyEvent {
    KeyAction action;
    char character;
};

/**
 * @brief Enum that represents the different possible states for extended and break scancodes
 */
enum ScancodeState {
    Normal,
    ExpectExtended,
    ExpectBreak,
    ExpectExtendedBreak,
};

class IKeyboard{
public:
    ~IKeyboard() = default;
    virtual void begin() = 0;
    virtual bool available() = 0;
    virtual KeyEvent readEvent() = 0;    
};