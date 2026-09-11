#pragma once

#include <string>
#include "hal/iterminal.h"
#include "utils/span.h"
#include "driver/uart.h"

// --- UART command codes ---
typedef enum {
    STREAM_CMD_SYNC_BYTE    = 0xFF,
    STREAM_CMD_ACK_BYTE     = 0xFE,
    STREAM_CMD_PRINT_CHAR   = 0x01,
    STREAM_CMD_PRINT_STRING = 0x02,
    STREAM_CMD_NEWLINE      = 0x03,
    STREAM_CMD_BACKSPACE    = 0x04,
    STREAM_CMD_REDRAW_LINE  = 0x05,
    STREAM_CMD_SET_CURSOR   = 0x10,
    STREAM_CMD_GET_CURSOR   = 0x11,
    STREAM_CMD_SET_CURSOR_VISIBLE = 0x12,
    STREAM_CMD_SET_FG       = 0x20,
    STREAM_CMD_SET_BG       = 0x21,
    STREAM_CMD_CLEAR        = 0x30,
    STREAM_CMD_SCROLL       = 0x31,
    STREAM_CMD_RESYNC       = 0x80,
} STREAM_CMD;

typedef struct {
    STREAM_CMD cmd;
    uint8_t len;
    uint8_t data[64];
    uint8_t seq;
} stream_msg;

#define UART_NUM                UART_NUM_1
#define UART_BUF_SIZE           1024
#define UART_MAX_PROTOCOL_SIZE  64
#define UART_PACKET_OVERHEAD    5   // SYNC, CMD, LEN, SEQ, CRC

#define UART_CMD_INDEX  1
#define UART_LEN_INDEX  2
#define UART_ARG_INDEX  3

class UARTTerminal : public ITerminal {
public:
    UARTTerminal(int baudRate, uint8_t txPin, uint8_t rxPin) : _baudRate(baudRate), _txPin(txPin), _rxPin(rxPin){};

    bool begin();
    
    // IO API calls
    bool sendChar(char c);
    bool print(const util::StringView txt);
    bool sendString(const util::span<char> txt);
    bool redrawLine(const util::span<char> line);
    bool setCursor(uint8_t x, uint8_t y);
    bool clearScreen();
    bool setBackground(uint8_t color);
    bool setColor(uint8_t color);
    bool backspace();
    bool newline();
    bool scroll(int8_t dir, uint8_t lines = 1);
    bool resync();

private:
    int _baudRate;
    uint8_t _txPin;
    uint8_t _rxPin;

    uint8_t _curSequenceNr;

    stream_msg _history[256];

    bool sendMessage(STREAM_CMD cmd, uint16_t length, const uint8_t* data);
    bool sendPacket(STREAM_CMD cmd, uint8_t length, const uint8_t* data);
};
