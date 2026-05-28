#include "uart_terminal.h"

bool UARTTerminal::begin() {
    uart_config_t uart_config = {
        .baud_rate  = _baudRate,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(UART_NUM, &uart_config);

    uart_set_pin(UART_NUM, UART_TX_PIN, UART_RX_PIN,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    if (uart_driver_install(UART_NUM, UART_BUF_SIZE, 0, 0, NULL, 0) != ESP_OK)
        return false;

    _curSequenceNr = 0;

    return true;
}

// Shared with the receiver
static inline uint8_t crc8_update(uint8_t crc, uint8_t data) {
    crc ^= data;
    for (int i = 0; i < 8; i++) {
        crc = (crc & 0x80) ? (crc << 1) ^ 0x07 : (crc << 1);
    }
    return crc;
}

// -------------------------------------------------------------------
//  Send exactly 3 bytes every time (cmd, lengh of data, data)
// -------------------------------------------------------------------
bool UARTTerminal::sendMessage(STREAM_CMD cmd, uint8_t length, const uint8_t* data) {
    stream_msg msg;
    msg.cmd = cmd;
    msg.len = length;
    memcpy(msg.data, data, length);

    uint8_t buffer[UART_BUF_SIZE + UART_PACKET_OVERHEAD];
    
    uint8_t crc = 0;
    buffer[0] = STREAM_CMD_SYNC_BYTE;
    buffer[UART_CMD_INDEX] = msg.cmd;
    crc = crc8_update(crc, msg.cmd);
    buffer[UART_LEN_INDEX] = msg.len;
    crc = crc8_update(crc, msg.len);

    if (msg.len > 0 && msg.data){
        memcpy(&buffer[UART_ARG_INDEX], msg.data, msg.len);
        for (int i = 0; i < msg.len; i++){
            crc = crc8_update(crc, *(msg.data + i));
        }
    }
    
    buffer[msg.len + UART_ARG_INDEX] = ++_curSequenceNr;
    _history[_curSequenceNr] = msg;
    crc = crc8_update(crc, _curSequenceNr);
    buffer[msg.len + UART_ARG_INDEX + 1] = crc;

    int written = uart_write_bytes(UART_NUM, buffer, UART_PACKET_OVERHEAD + msg.len);

    return written == (UART_PACKET_OVERHEAD + msg.len);
}

// -------------------------------------------------------------------
//  API implementation (unchanged API, new internals)
// -------------------------------------------------------------------
bool UARTTerminal::sendChar(char c) {
    uint8_t b = (uint8_t)c;
    return sendMessage(STREAM_CMD_PRINT_CHAR, 1, &b);
}

bool UARTTerminal::sendString(const std::string& txt) {
    return sendMessage(
        STREAM_CMD_PRINT_STRING,
        txt.size(),
        reinterpret_cast<const uint8_t*>(txt.data())
    );
}

bool UARTTerminal::redrawLine(const std::string& line) {
    return sendMessage(
        STREAM_CMD_REDRAW_LINE,
        line.size(),
        reinterpret_cast<const uint8_t*>(line.data())
    );
}

bool UARTTerminal::setCursor(uint8_t x, uint8_t y) {
    uint8_t pos[2] = {x, y};
    return sendMessage(STREAM_CMD_SET_CURSOR, 2, pos);
}

bool UARTTerminal::clearScreen() {
    return sendMessage(STREAM_CMD_CLEAR, 0, nullptr);
}

bool UARTTerminal::setBackground(uint8_t color) {
    return sendMessage(STREAM_CMD_SET_BG, 1, &color);
}

bool UARTTerminal::scroll(int8_t dir, uint8_t lines) {
    uint8_t packet[2] = { (uint8_t)dir, lines };
    return sendMessage(STREAM_CMD_SCROLL, 2, packet);
}

bool UARTTerminal::backspace(){
    return sendMessage(STREAM_CMD_BACKSPACE, 0, nullptr);
}

bool UARTTerminal::newline(){
    return sendMessage(STREAM_CMD_NEWLINE, 0, nullptr);
}

bool UARTTerminal::resync(){
    return sendMessage(STREAM_CMD_RESYNC, 0, nullptr);
}