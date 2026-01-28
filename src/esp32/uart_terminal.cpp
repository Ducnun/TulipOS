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

    return true;
}

// -------------------------------------------------------------------
//  Send exactly 3 bytes every time (cmd, lengh of data, data)
// -------------------------------------------------------------------
bool UARTTerminal::sendMessage(uint8_t cmd, uint8_t length, const uint8_t* data) {
    uint8_t buffer[2 + UART_BUF_SIZE];

    if (length > UART_BUF_SIZE) return false;

    buffer[0] = cmd;
    buffer[1] = length;

    if (length > 0 && data)
        memcpy(&buffer[2], data, length);

    int written = uart_write_bytes(UART_NUM, buffer, 2 + length);

    return written == (2 + length);
}

// -------------------------------------------------------------------
//  API implementation (unchanged API, new internals)
// -------------------------------------------------------------------
bool UARTTerminal::sendChar(char c) {
    uint8_t b = (uint8_t)c;
    return sendMessage(CMD_PRINT_CHAR, 1, &b);
}

bool UARTTerminal::sendString(const std::string& txt) {
    return sendMessage(
        CMD_PRINT_STRING,
        txt.size(),
        reinterpret_cast<const uint8_t*>(txt.data())
    );
}

bool UARTTerminal::redrawLine(const std::string& line) {
    return sendMessage(
        CMD_REDRAW_LINE,
        line.size(),
        reinterpret_cast<const uint8_t*>(line.data())
    );
}

bool UARTTerminal::setCursor(uint8_t x, uint8_t y) {
    uint8_t pos[2] = {x, y};
    return sendMessage(CMD_SET_CURSOR, 2, pos);
}

bool UARTTerminal::clearScreen() {
    return sendMessage(CMD_CLEAR, 0, nullptr);
}

bool UARTTerminal::setBackground(uint8_t color) {
    return sendMessage(CMD_SET_BG, 1, &color);
}

bool UARTTerminal::scroll(int8_t dir, uint8_t lines) {
    uint8_t packet[2] = { (uint8_t)dir, lines };
    return sendMessage(CMD_SCROLL, 2, packet);
}