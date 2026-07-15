#pragma once

// Basic commands
#define SD_SPI_GO_IDLE_STATE        0
#define SD_SPI_SEND_OP_COND         1
#define SD_SPI_SEND_IF_COND         8
#define SD_SPI_SEND_CSD             9
#define SD_SPI_SEND_CID             10
#define SD_SPI_STOP_TRANSMISSION    12
#define SD_SPI_SEND_STATUS          13
#define SD_SPI_SET_BLOCKLEN         16
#define SD_SPI_READ_SINGLE_BLOCK    17
#define SD_SPI_READ_MULTIPLE_BLOCK  18
#define SD_SPI_SET_BLOCK_COUNT      23
#define SD_SPI_WRITE_BLOCK          24
#define SD_SPI_WRITE_MULTIPLE_BLOCK 25
#define SD_SPI_READ_OCR             58

// Appended commands
#define SD_SPI_APP_CMD                          55
#define SD_SPI_APP_SET_WR_BLOCK_ERASE_COUNT     23
#define SD_SPI_APP_SEND_OP_COND                 41

// Response values
#define SD_SPI_R1_IN_IDLE_STATE     (1u << 0)
#define SD_SPI_R1_ERASE_RESET       (1u << 1)
#define SD_SPI_R1_ILLEGAL_COMMAND   (1u << 2)
#define SD_SPI_R1_COMMAND_CRC_ERROR (1u << 3)
#define SD_SPI_R1_ERASE_SEQ_ERROR   (1u << 4)
#define SD_SPI_R1_ADDRESS_ERROR     (1u << 5)
#define SD_SPI_R1_PARAMETER_ERROR   (1u << 6)

// Command Args
#define SD_SPI_HCS      (1u << 30)