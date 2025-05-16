#include "ps2_keyboard.h"

PS2Keyboard* instancePtr = nullptr;

PS2Keyboard::PS2Keyboard(uint8_t dataPin, uint8_t clockPin)
    : _dataPin(dataPin), _clockPin(clockPin) {
    instancePtr = this;
}

void PS2Keyboard::begin(){
    pinMode(_clockPin, INPUT_PULLUP);
    pinMode(_dataPin, INPUT_PULLUP);
    attachInterruptArg(digitalPinToInterrupt(_clockPin), handleClockInterrupt, this, FALLING);
    pinMode(22, OUTPUT);
}

void IRAM_ATTR PS2Keyboard::handleClockInterrupt(void* arg){
    digitalWrite(22, !digitalRead(22));
    reinterpret_cast<PS2Keyboard*>(arg)->onClockFallingEdge();
}

void PS2Keyboard::onClockFallingEdge(){
    static uint8_t bitBuffer = 0;
    static uint8_t parity = 0;

    uint8_t bit = digitalRead(_dataPin);

    if (_bitCount == 0){
        // Reset when start bit is sent
        if (bit == 0){
            _receiving = true;
            bitBuffer = 0;
            parity = 0;
            _bitCount++;
        }
    } 
    else if (_bitCount >= 1 && _bitCount <= 8){
        bitBuffer |= (bit << (_bitCount - 1));
        parity ^= bit;
        _bitCount++;
    }
    else if (_bitCount == 9){
        // Parity bit
        _bitCount++;
    }
    else if (_bitCount == 10){
        if (_receiving){
            portENTER_CRITICAL_ISR(&_mux);
            _scancodeBuffer.push(bitBuffer);
            portEXIT_CRITICAL_ISR(&_mux);
        }
        _bitCount = 0;
        _receiving = false;
    }
}

bool PS2Keyboard::available(){
    return !_scancodeBuffer.empty();
}

char PS2Keyboard::read(){
    while (!_scancodeBuffer.empty()) {
        portENTER_CRITICAL(&_mux);
        uint8_t code = _scancodeBuffer.front();
        _scancodeBuffer.pop();
        portEXIT_CRITICAL(&_mux);

        static bool breakCodeNext = false;
        if (code == 0xF0) {
            breakCodeNext = true;
            continue;  // don't return anything
        }
        if (breakCodeNext) {
            handleKeyReleaseEvent(code);
            breakCodeNext = false;
            continue;  // also don't return anything
        }

        if (handleKeyPressEvent(code)){
            char out = translateScancode(code);
    
            if (out != '\0') {
                return out;
            }
        }
        // else loop to try again
    }
    return '\0';
}

char PS2Keyboard::translateScancode(uint8_t code){
    static std::unordered_map<uint8_t, char> baseMap = {
        {0x1C, 'a'}, {0x32, 'b'}, {0x21, 'c'}, {0x23, 'd'}, {0x24, 'e'}, {0x2B, 'f'}, {0x34, 'g'}, {0x33, 'h'}, {0x43, 'i'},
        {0x3B, 'j'}, {0x42, 'k'}, {0x4B, 'l'}, {0x3A, 'm'}, {0x31, 'n'}, {0x44, 'o'}, {0x4D, 'p'}, {0x15, 'q'}, {0x2D, 'r'},
        {0x1B, 's'}, {0x2C, 't'}, {0x3C, 'u'}, {0x2A, 'v'}, {0x1D, 'w'}, {0x22, 'x'}, {0x35, 'y'}, {0x1A, 'z'},
        {0x29, ' '}, {0x5A, '\n'}, {0x66, '\b'}, 
        {0x52, '\''}, {0x4E, '-'}, {0x55, '='}, {0x4C, ';'}, {0x4A, '/'}, {0x49, '.'}, {0x41, ','}      // Also handled in shift map
    };

    static std::unordered_map<uint8_t, char> shiftMap = {
        {0x52, '"'}, {0x4E, '_'}, {0x55, '+'}, {0x4C, ':'}, {0x4A, '?'}, {0x49, '>'}, {0x41, '<'}
    };

    if (shiftHeld){
        auto it = shiftMap.find(code);
        if (it != shiftMap.end()) return it->second;
    }

    auto it = baseMap.find(code);
    if (it == baseMap.end()) return '?';

    char c = it->second;
    if (c >= 'a' && c <= 'z'){
        if (capsLockActive ^ shiftHeld){
            c = c - 'a' + 'A';
        }
    }

    return c;
}

// Returns whether a key is printable or not
bool PS2Keyboard::handleKeyPressEvent(uint8_t code){
    if (code == 0x12 || code == 0x59) {shiftHeld = true; return false; }
    if (code == 0x11 || code == 0x58) {ctrlHeld = true; return false; }
    if (code == 0x19 || code == 0x11) {altHeld = true; return false; }
    if (code == 0x58) {capsLockActive = !capsLockActive; return false; } 
    return true;
}

void PS2Keyboard::handleKeyReleaseEvent(uint8_t code){
    if (code == 0x12 || code == 0x59) shiftHeld = false;
    if (code == 0x11 || code == 0x58) ctrlHeld = false;
    if (code == 0x19 || code == 0x11) altHeld = false;
}