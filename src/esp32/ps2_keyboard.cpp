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

            // DEBUGGING LINES
            debugCodes[debugWriteIndex] = bitBuffer;
            debugWriteIndex = (debugWriteIndex + 1) % 32;

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
    if (_scancodeBuffer.empty()) return 0;

    portENTER_CRITICAL(&_mux);
    uint8_t code = _scancodeBuffer.front();
    _scancodeBuffer.pop();
    portEXIT_CRITICAL(&_mux);

    return translateScancode(code);
}

char PS2Keyboard::translateScancode(uint8_t code){
    static std::unordered_map<uint8_t, char> map = {
        {0x1C, 'A'}, {0x32, 'B'}, {0x4D, 'P'}, {0xF0, '_'}, {0x5A, '\n'}        
    };
    return map.count(code) ? map[code] : '?';
}

void PS2Keyboard::dumpDebugPS2(){
    while (debugReadIndex != debugWriteIndex){
        uint8_t code = debugCodes[debugReadIndex];
        debugReadIndex = (debugReadIndex + 1) % 32;
        Serial.print("Scancode (from buffer): 0x");
        Serial.println(code, HEX);
    }
}