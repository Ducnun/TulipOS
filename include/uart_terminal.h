#pragma once

#include <Arduino.h>
#include "driver/uart.h"

// --- UART command codes ---
enum UARTCmd : uint8_t {
    CMD_PRINT_CHAR = 1,
    CMD_SET_CURSOR = 2,
    CMD_SCROLL     = 3,
    CMD_SET_BG     = 4,
    CMD_CLEAR      = 5,
    CMD_PRINT_STRING = 6,
    CMD_REDRAW_LINE = 7,
};

#define UART_NUM        UART_NUM_1
#define UART_TX_PIN     17
#define UART_RX_PIN     16
#define UART_BUF_SIZE   1024

class UARTTerminal {
public:
    UARTTerminal(int baudRate) : _baudRate(baudRate){};

    bool begin();
    
    // IO API calls
    bool sendChar(char c);
    bool sendString(const std::string& txt);
    bool redrawLine(const std::string& line);
    bool setCursor(uint8_t x, uint8_t y);
    bool clearScreen();
    bool setBackground(uint8_t color);
    
    bool scroll(int8_t dir, uint8_t lines = 1);
private:
    int _baudRate;

    bool sendMessage(uint8_t cmd, uint8_t length, const uint8_t* data);
};
