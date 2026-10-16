#include "ps2_keyboard.h"

#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_attr.h"
#include "esp_log.h"

PS2Keyboard* instancePtr = nullptr;
static const char* TAG = "PS2Keyboard";

void IRAM_ATTR PS2Keyboard::handleClockInterrupt(void* arg) {
    PS2Keyboard* obj = reinterpret_cast<PS2Keyboard*>(arg);
    
    static int led_state = 0;
    led_state = !led_state;
    gpio_set_level((gpio_num_t)3, led_state);

    obj->onClockFallingEdge();
}

PS2Keyboard::PS2Keyboard(uint8_t dataPin, uint8_t clockPin){
    _dataPin = dataPin;
    _clockPin = clockPin;
    instancePtr = this;
}

void PS2Keyboard::begin(){
    // Setup ISR safety
    _mux = portMUX_INITIALIZER_UNLOCKED;
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);

    // Configure Clock and Data pins
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_NEGEDGE; // FALLING
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << _clockPin) | (1ULL << _dataPin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    // Set LED for debugging purposes
    gpio_reset_pin((gpio_num_t)3);
    gpio_set_direction((gpio_num_t)3, GPIO_MODE_OUTPUT);
    // Attach ISR
    gpio_isr_handler_add((gpio_num_t)_clockPin, handleClockInterrupt, (void*)this);

    setLEDs(false, true, false);
}

bool PS2Keyboard::available(){
    return _head != _tail;
}
KeyEvent PS2Keyboard::readEvent() {
    if (!available()) {
        return {None, 0}; // No key available
    }

    portENTER_CRITICAL(&_mux);
    uint8_t code = _scancodeBuffer[_tail];
    _tail = (_tail + 1) % BufferSize;
    portEXIT_CRITICAL(&_mux);

    if (trackPauseState(code)) return {Pause, 0};
    if (isPauseSequenceInProgress()) return {None, 0}; //Yield and retry

    switch (_scancodeState) {
        case ScancodeState::Normal:
            if (code == 0xE0) {
                _scancodeState = ScancodeState::ExpectExtended;
                return {None, 0}; // Need another byte
            } else if (code == 0xF0) {
                _scancodeState = ScancodeState::ExpectBreak;
                return {None, 0}; // Need another byte
            }
            if (handleKeyPressEvent(code)) {
                KeyEvent ev = translateScancode(code);
                _scancodeState = ScancodeState::Normal;
                return ev;
            }
            break;
        // Handle other states similarly...
        case ScancodeState::ExpectExtended:
            if (code == 0xF0) {
                _scancodeState = ScancodeState::ExpectExtendedBreak;
                return {None, 0};
            }
            if (code == 0x12 || code == 0x59) { // Skip phantom Shift
                _scancodeState = ScancodeState::Normal;
                return {None, 0};
            }
            _scancodeState = ScancodeState::Normal;
            if (handleKeyPressEvent(code)) {
                return translateExtendedScancode(code);
            }
            break;
        case ScancodeState::ExpectBreak:
            handleKeyReleaseEvent(code, false);
            _scancodeState = ScancodeState::Normal;
            break;
        case ScancodeState::ExpectExtendedBreak:
            if (code == 0x12 || code == 0x59) { // Skip phantom Shift
                _scancodeState = ScancodeState::Normal;
                return {None, 0};
            }
            handleKeyReleaseEvent(code, true);
            _scancodeState = ScancodeState::Normal;
            break;
    }

    return {None, 0}; // No event ready yet
}

void PS2Keyboard::onClockFallingEdge(){
    uint8_t bit = gpio_get_level((gpio_num_t)_dataPin);

    if (_bitCount == 0){
        // Reset when start bit is sent
        if (bit == 0){
            _receiving = true;
            _currentByte = 0;
            _parity = 0;
            _bitCount += 1;
        }
    } 
    else if (_bitCount >= 1 && _bitCount <= 8){
        // Data bits: LSB first
        _currentByte |= (bit << (_bitCount - 1));
        _parity ^= bit;
        _bitCount += 1;
    }
    else if (_bitCount == 9){
        // Parity bit
        _bitCount += 1;
    }
    else if (_bitCount == 10){
        // Stop bit: Should be a 1
        if (_receiving){
            size_t nextHead = (_head + 1) % BufferSize;
            if (nextHead != _tail){
                portENTER_CRITICAL_ISR(&_mux);
                _scancodeBuffer[_head] = _currentByte;
                _head = nextHead;
                portEXIT_CRITICAL_ISR(&_mux);
            }
        }
        _bitCount = 0;
        _receiving = false;
    }
}

KeyEvent PS2Keyboard::translateScancode(uint8_t code){
    static std::unordered_map<uint8_t, char> baseMap = {
        {0x1C, 'a'}, {0x32, 'b'}, {0x21, 'c'}, {0x23, 'd'}, {0x24, 'e'}, {0x2B, 'f'}, {0x34, 'g'}, {0x33, 'h'}, {0x43, 'i'},
        {0x3B, 'j'}, {0x42, 'k'}, {0x4B, 'l'}, {0x3A, 'm'}, {0x31, 'n'}, {0x44, 'o'}, {0x4D, 'p'}, {0x15, 'q'}, {0x2D, 'r'},
        {0x1B, 's'}, {0x2C, 't'}, {0x3C, 'u'}, {0x2A, 'v'}, {0x1D, 'w'}, {0x22, 'x'}, {0x35, 'y'}, {0x1A, 'z'},
        {0x29, ' '}, {0x5A, '\n'}, {0x66, '\b'}, 
        {0x52, '\''}, {0x4E, '-'}, {0x55, '='}, {0x4C, ';'}, {0x4A, '/'}, {0x49, '.'}, {0x41, ','},      // Also handled in shift map
        {0x16, '1'}, {0x1E, '2'}, {0x26, '3'}, {0x25, '4'}, {0x2E, '5'}, {0x36, '6'}, {0x3D, '7'}, {0x3E, '8'}, {0x46, '9'}, {0x45, '0'}
    };

    static std::unordered_map<uint8_t, char> shiftMap = {
        {0x52, '"'}, {0x4E, '_'}, {0x55, '+'}, {0x4C, ':'}, {0x4A, '?'}, {0x49, '>'}, {0x41, '<'},
        {0x16, '!'}, {0x1E, '@'}, {0x26, '#'}, {0x25, '$'}, {0x2E, '%'}, {0x36, '^'}, {0x3D, '&'}, {0x3E, '*'}, {0x46, '('}, {0x45, ')'},
        {0x54, '{'}, {0x5B, '}'}
    };

    if (_shiftHeld){
        auto it = shiftMap.find(code);
        if (it != shiftMap.end()) {
            return {KeyAction::Char, it->second};
        }
    }

    auto it = baseMap.find(code);
    if (it == baseMap.end()){
        return {KeyAction::None, 0};
    };

    char c = it->second;
    if (c >= 'a' && c <= 'z'){
        if (_capsLockActive ^ _shiftHeld){
            c = c - 'a' + 'A';
        }
    }

    return {KeyAction::Char, c};
}

KeyEvent PS2Keyboard::translateExtendedScancode(uint8_t code){
    static std::unordered_map<uint8_t, KeyAction> extendedMap = {
        {0x6B, KeyAction::ArrowLeft}, {0x74, KeyAction::ArrowRight}, {0x75, KeyAction::ArrowUp}, {0x72, KeyAction::ArrowDown}
    };

    auto it = extendedMap.find(code);
    if (it != extendedMap.end()){
        return {extendedMap[code], 0};
    }
    else{
        return {KeyAction::None, 0};
    }
}

bool PS2Keyboard::handleKeyPressEvent(uint8_t code){
    // Ensure that extended sequences dont set unintended flags
    if (_scancodeState == ScancodeState::ExpectExtended) return true;
    if (code == 0x12) {_shiftHeld = true; return false;}
    if (code == 0x14) {_ctrlHeld = true; return false; }
    if (code == 0x11) {_altHeld = true; return false; }
    if (code == 0x58) {_capsLockActive = !_capsLockActive; setLEDs(_capsLockActive, _numLockActive, false); return false; } 
    return true;
}

void PS2Keyboard::handleKeyReleaseEvent(uint8_t code, bool extended){
    if (extended){
        // Handle extended releases if needed
        if (code == 0x59) _shiftHeld = false;
        if (code == 0x14) _ctrlHeld = false;
        if (code == 0x11) _altHeld = false;
        return;
    }
    if (code == 0x12) {_shiftHeld = false;};
    if (code == 0x14) _ctrlHeld = false;
    if (code == 0x11) _altHeld = false;
}

uint8_t PS2Keyboard::readRawScancode(){
    if (_head == _tail) return 0;
    portENTER_CRITICAL(&_mux);
    uint8_t code = _scancodeBuffer[_tail];
    _tail = (_tail + 1) % BufferSize;
    portEXIT_CRITICAL(&_mux);
    return code;
}

bool PS2Keyboard::waitForClockLevel(int level) {
    const int64_t timeoutUs = 100000;
    const int64_t startTime = esp_timer_get_time();

    while (gpio_get_level((gpio_num_t)_clockPin) != level) {
        if (esp_timer_get_time() - startTime >= timeoutUs) return false;
        esp_rom_delay_us(10);
    }

    return true;
}

void PS2Keyboard::releaseBusAfterTransmit() {
    gpio_set_direction((gpio_num_t)_dataPin, GPIO_MODE_INPUT);
    gpio_set_pull_mode((gpio_num_t)_dataPin, GPIO_PULLUP_ONLY);
    gpio_set_direction((gpio_num_t)_clockPin, GPIO_MODE_INPUT);
    gpio_set_pull_mode((gpio_num_t)_clockPin, GPIO_PULLUP_ONLY);
    gpio_intr_enable((gpio_num_t)_clockPin);
}

bool PS2Keyboard::sendByteToKeyboard(uint8_t data) {
    // 1. Disable the clock interrupt so we don't "receive" our own transmission
    gpio_intr_disable((gpio_num_t)_clockPin);

    // --- REQUEST TO SEND (Inhibit) ---
    gpio_set_direction((gpio_num_t)_clockPin, GPIO_MODE_OUTPUT);
    gpio_set_level((gpio_num_t)_clockPin, 0);
    esp_rom_delay_us(100);
    gpio_set_direction((gpio_num_t)_dataPin, GPIO_MODE_OUTPUT);
    gpio_set_level((gpio_num_t)_dataPin, 0);
    esp_rom_delay_us(10);
    gpio_set_direction((gpio_num_t)_clockPin, GPIO_MODE_INPUT);
    gpio_set_pull_mode((gpio_num_t)_clockPin, GPIO_PULLUP_ONLY);

    // Wait for keyboard to begin generating clock (Wait for falling edge)
    if (!waitForClockLevel(0)) {
        releaseBusAfterTransmit();
        return false;
    }

    uint8_t parity = 1;
    for (int i = 0; i < 8; i++) {
        bool bit = (data >> i) & 1;
        
        gpio_set_level((gpio_num_t)_dataPin, bit);
        parity ^= bit;

        if (!waitForClockLevel(1) || !waitForClockLevel(0)) {
            releaseBusAfterTransmit();
            return false;
        }
    }

    // --- PARITY BIT ---
    gpio_set_level((gpio_num_t)_dataPin, parity);
    if (!waitForClockLevel(1) || !waitForClockLevel(0)) {
        releaseBusAfterTransmit();
        return false;
    }

    // --- STOP BIT ---
    // Release data line
    gpio_set_direction((gpio_num_t)_dataPin, GPIO_MODE_INPUT);
    gpio_set_pull_mode((gpio_num_t)_dataPin, GPIO_PULLUP_ONLY);
    
    if (!waitForClockLevel(1) || !waitForClockLevel(0)) {
        releaseBusAfterTransmit();
        return false;
    }

    // --- WAIT FOR ACK (0xFA) ---
    
    // Re-enable interrupts so available() and readRawScancode() work again
    releaseBusAfterTransmit();

    // millis() equivalent: esp_timer_get_time() returns microseconds
    int64_t startTime = esp_timer_get_time();
    while ((esp_timer_get_time() - startTime) < 100000) { // 100ms = 100,000us
        if (available()) {
            uint8_t ack = readRawScancode();
            if (ack == 0xFA) return true;
        }
        // Small yield to keep the Watchdog happy during the 100ms wait
        vTaskDelay(1); 
    }

    return false;
}

void PS2Keyboard::setLEDs(bool caps, bool num, bool scroll) {
    if (!sendByteToKeyboard(0xED)) {
        ESP_LOGW(TAG, "Keyboard did not acknowledge LED command");
        return;
    }
    vTaskDelay(pdMS_TO_TICKS(1));
    uint8_t val = (scroll ? 1 : 0) | (num ? 2 : 0) | (caps ? 4 : 0);
    if (!sendByteToKeyboard(val)) {
        ESP_LOGW(TAG, "Keyboard did not acknowledge LED state");
    }
}

bool PS2Keyboard::trackPauseState(uint8_t code) {
    switch (pauseState) {
        case 0: if (code == 0xE1) pauseState = 1; else pauseState = 0; break;
        case 1: if (code == 0x14) pauseState = 2; else pauseState = 0; break;
        case 2: if (code == 0x77) pauseState = 3; else pauseState = 0; break;
        case 3: if (code == 0xE1) pauseState = 4; else pauseState = 0; break;
        case 4: if (code == 0xF0) pauseState = 5; else pauseState = 0; break;
        case 5: if (code == 0x14) pauseState = 6; else pauseState = 0; break;
        case 6: if (code == 0xF0) pauseState = 7; else pauseState = 0; break;
        case 7:
            if (code == 0x77) {
                pauseState = 0;
                return true; // COMPLETE
            }
            pauseState = 0;
            break;
    }

    return false;
}

bool PS2Keyboard::isPauseSequenceInProgress() {
    return pauseState != 0;
}

void PS2Keyboard::debugPrintState(const char* context, uint8_t code) {
    printf("[");
    printf(context);
    printf("] code=0x");
    printf("%x", code);
    printf("  state=");

    switch (_scancodeState) {
        case ScancodeState::Normal: printf("Normal"); break;
        case ScancodeState::ExpectExtended: printf("ExpectExtended"); break;
        case ScancodeState::ExpectBreak: printf("ExpectBreak"); break;
        case ScancodeState::ExpectExtendedBreak: printf("ExpectExtendedBreak"); break;
    }

    printf("  flags: ");
    printf(_shiftHeld ? "S" : "-");
    printf(_ctrlHeld ? "C" : "-");
    printf(_altHeld ? "A" : "-");
    printf(_capsLockActive ? "L" : "-");
    printf("\n");
}