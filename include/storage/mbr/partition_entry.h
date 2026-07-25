#pragma once

#include <cstdint>

struct PartitionEntry{
    uint8_t     bootFlag;
    uint8_t     chsStart[3];
    uint8_t     type;
    uint8_t     chsEnd[3];
    uint32_t    firstLBA;
    uint32_t    sectorCount;
};