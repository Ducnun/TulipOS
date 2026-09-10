#pragma once

#include <stdint.h>
#include <cstddef>
#include <queue>
#include <unordered_map> 
#include <string>
#include "freertos/FreeRTOS.h"

#include "hal/ikeyboard.h"

/**
 * @brief Enum that represents the different possible states for extended and break scancodes
 */
enum ScancodeState {
    Normal,
    ExpectExtended,
    ExpectBreak,
    ExpectExtendedBreak,
};

class PS2Keyboard : public IKeyboard{
public:
    /**
     * @brief Construct a new PS2Keyboard object
     * @param dataPin Pin that the PS2 keyboards' data line is connected to
     * @param clockPin Pin that the PS2 keyboards' clock line is connected to
     */
    PS2Keyboard(uint8_t dataPin, uint8_t clockPin);

    /**
     * @brief Initializes the keyboard object, configuring pins, LEDs and interruptss
     */
    void begin();

    /**
     * @brief Checks whether the buffer holds any values
     * @return true Buffer holds a value
     * @return false Buffer is empty
     */
    bool available();

    /**
     * @brief Registers the keyevent if available
     * @return KeyEvent that holds the action as well as the character if applicable
     */
    KeyEvent readEvent();

private:
    /**
     * @brief Registers a memory safe interrupt on the clock signal
     * @param arg Callback function to register
     */
    static void handleClockInterrupt(void* arg);

    /**
     * @brief Callback function for the clock signal
     */
    void onClockFallingEdge();

    uint8_t _dataPin;   ///< Datapin for the PS2 keyboard
    uint8_t _clockPin;  ///< Clockpin for the PS2 keyboard
    
    // Start reading inputs after interrupt
    volatile uint8_t _bitCount = 0; ///< Counts the amount of bits read of the current message
    volatile uint8_t _currentByte = 0;  ///< Current byte being constructed
    volatile uint8_t _parity = 0;   ///< Holds the parity bit (not yet implemented)
    volatile bool _receiving = false;   ///< Tracks whether the keyboard is currently receiving a message
    ScancodeState _scancodeState;   ///< Tracks the scancode state, needed for extended and breaks
    uint8_t pauseState = 0; ///< Tracks the specific message of the pause/break code

    // Track state of modifiers
    bool _capsLockActive = false;   ///< Tracks the caps lock state
    bool _numLockActive = true; ///< Tracks num lock state
    bool _shiftHeld = false;    ///< Tracks whether shift is held
    bool _ctrlHeld = false; ///< Tracks whether control is held
    bool _altHeld = false;  ///< Tracks whether alt is held
    bool _extendedCodeNext = false; ///< Tracks whether an extended code is coming next
    bool _breakCodeNext = false;    ///< Tracks whether a break code is coming next

    static const size_t BufferSize = 32;    ///< Cyclic buffer size
    volatile uint8_t _scancodeBuffer[BufferSize];   ///< Cyclic buffer that stores key presses in interrupt
    volatile size_t _head = 0;  ///< Tracks the head of the buffer
    volatile size_t _tail = 0;  ///< Tracks the tail of the buffer
    portMUX_TYPE _mux = portMUX_INITIALIZER_UNLOCKED;   /// Mutual Exclusion to make interrupt memory safe

    /**
     * @brief Translates base scancodes as well as shift modified codes
     * @param code Raw byte to translate to a character
     * @return KeyEvent The keyevent with action and char if applicable
     */
    KeyEvent translateScancode(uint8_t code);

    /**
     * @brief Translates an extended code
     * @param code Code without the extended code bytes
     * @return KeyEvent The keyevent with action and char if applicable
     */
    KeyEvent translateExtendedScancode(uint8_t code);

    /**
     * @brief Checks if a key press should result in a state change or an output char
     * @param code Scancode to read out
     * @return true If code press results in a printable character
     * @return false If code just updates the state state of the PS2 object
     */
    bool handleKeyPressEvent(uint8_t code);
    
    /**
     * @brief Handles a release event if the next code is a break code
     * @param code Scancode that is being broken
     * @param extended Whether the code being broken is an extended code
     */
    void handleKeyReleaseEvent(uint8_t code, bool extended);

    /**
     * @brief Returns the raw byte instead of translating directly
     * @return uint8_t Raw scancode
     */
    uint8_t readRawScancode();

    /**
     * @brief Sends a byte to the keyboard by taking over communication
     * @param data Byte sent to the keyboard
     */
    void sendByteToKeyboard(uint8_t data);

    /**
     * @brief Sets the LEDs on the keyboard
     * @param caps State of caps lock
     * @param num State of num lock
     * @param scroll State of scroll lock
     */
    void setLEDs(bool caps, bool num, bool scroll);
    
    /**
     * @brief Tracks the exact code sequence of a pause/break key press
     * @param code Current code
     * @return true if a full sequence of pause codes is tracked
     * @return false if the pause code is not fully read or broken
     */
    bool trackPauseState(uint8_t code);

    /**
     * @brief Tracks whether a pause sequence is in progress
     * @return true if a start is tracked and not broken off yet
     * @return false if pause sequence is not in progress
     */
    bool isPauseSequenceInProgress();

    /**
     * @brief Prints raw frames and the current scancodestate
     * 
     */
    void debugPrintState(const char* context, uint8_t code);
};
