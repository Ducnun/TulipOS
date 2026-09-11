#include "uart_terminal.h"

bool UARTTerminal::begin() {
    const uart_port_t uart_num = UART_NUM;

    // 1. Zero-initialize the struct first to satisfy -Werror
    uart_config_t uart_config = {}; 
    
    // 2. Set the fields individually (Order doesn't matter here)
    uart_config.baud_rate = _baudRate;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.rx_flow_ctrl_thresh = 122;
    uart_config.source_clk = UART_SCLK_DEFAULT;

    // 3. Configure parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

    // 4. Set UART pins (TX, RX, RTS, CTS)
    ESP_ERROR_CHECK(uart_set_pin(uart_num, _txPin, _rxPin, 
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    if (uart_driver_install(uart_num, UART_BUF_SIZE * 2, 0, 0, NULL, 0) != ESP_OK) {
        return false;
    }

    _curSequenceNr = 0;
    return true;
}

static inline uint8_t crc8_update(uint8_t crc, uint8_t data) {
    crc ^= data;
    for (int i = 0; i < 8; i++) {
        crc = (crc & 0x80) ? (crc << 1) ^ 0x07 : (crc << 1);
    }
    return crc;
}

bool UARTTerminal::sendMessage(STREAM_CMD cmd, uint16_t length, const uint8_t* data) {
    constexpr uint16_t MAX_PAYLOAD = UART_MAX_PROTOCOL_SIZE;
    uint16_t offset = 0;

    while (offset < length) {
        uint16_t chunk = std::min(MAX_PAYLOAD, static_cast<uint16_t>(length - offset));
        if (!sendPacket(cmd, (uint8_t)chunk, data + offset)) {
            return false;
        }
        offset += chunk;
    }
    return true;
}

bool UARTTerminal::sendPacket(STREAM_CMD cmd, uint8_t length, const uint8_t* data) {
    if (!uart_is_driver_installed(UART_NUM)) {
        printf("CRITICAL: UART driver not installed for port %d!\n", UART_NUM);
        return false;
    }
    uint8_t buffer[UART_BUF_SIZE + UART_PACKET_OVERHEAD];
    uint8_t crc = 0;
    
    // Header
    buffer[0] = STREAM_CMD_SYNC_BYTE;
    buffer[UART_CMD_INDEX] = (uint8_t)cmd;
    crc = crc8_update(crc, (uint8_t)cmd);
    
    buffer[UART_LEN_INDEX] = length;
    crc = crc8_update(crc, length);

    // Payload
    if (length > 0 && data) {
        memcpy(&buffer[UART_ARG_INDEX], data, length);
        for (int i = 0; i < length; i++) {
            crc = crc8_update(crc, data[i]);
        }
    }
    
    // Sequence and CRC Footer
    buffer[length + UART_ARG_INDEX] = ++_curSequenceNr;
    crc = crc8_update(crc, _curSequenceNr);
    
    buffer[length + UART_ARG_INDEX + 1] = crc;

    // Send via IDF Driver
    int total_len = length + UART_PACKET_OVERHEAD;
    int written = uart_write_bytes(UART_NUM, buffer, total_len);

    return written == total_len;
}

// -------------------------------------------------------------------
//  API implementation (unchanged API, new internals)
// -------------------------------------------------------------------
bool UARTTerminal::sendChar(char c) {
    uint8_t b = (uint8_t)c;
    return sendPacket(STREAM_CMD_PRINT_CHAR, 1, &b);
}

bool UARTTerminal::print(const util::StringView txt){
    return sendMessage(
        STREAM_CMD_PRINT_STRING,
        txt.length,
        reinterpret_cast<const uint8_t*>(txt.data)
    );
}

bool UARTTerminal::sendString(const util::span<char> txt) {
    return sendMessage(
        STREAM_CMD_PRINT_STRING,
        txt.size(),
        reinterpret_cast<const uint8_t*>(txt.data())
    );
}

bool UARTTerminal::redrawLine(const util::span<char> line) {
    return sendMessage(
        STREAM_CMD_REDRAW_LINE,
        line.size(),
        reinterpret_cast<const uint8_t*>(line.data())
    );
}

bool UARTTerminal::setCursor(uint8_t x, uint8_t y) {
    uint8_t pos[2] = {x, y};
    return sendPacket(STREAM_CMD_SET_CURSOR, 2, pos);
}

bool UARTTerminal::clearScreen() {
    return sendPacket(STREAM_CMD_CLEAR, 0, nullptr);
}

bool UARTTerminal::setBackground(uint8_t color) {
    return sendPacket(STREAM_CMD_SET_BG, 1, &color);
}

bool UARTTerminal::setColor(uint8_t color) {
    return sendPacket(STREAM_CMD_SET_FG, 1, &color);
}

bool UARTTerminal::scroll(int8_t dir, uint8_t lines) {
    uint8_t packet[2] = { (uint8_t)dir, lines };
    return sendPacket(STREAM_CMD_SCROLL, 2, packet);
}

bool UARTTerminal::backspace(){
    return sendPacket(STREAM_CMD_BACKSPACE, 0, nullptr);
}

bool UARTTerminal::newline(){
    return sendPacket(STREAM_CMD_NEWLINE, 0, nullptr);
}

bool UARTTerminal::resync(){
    return sendPacket(STREAM_CMD_RESYNC, 0, nullptr);
}