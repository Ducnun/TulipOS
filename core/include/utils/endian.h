#pragma once

#include <stdint.h>

namespace util {
    inline uint16_t readLE16(const uint8_t* ptr) {
        uint16_t res;
        res = ptr[0];
        res |= (uint16_t)ptr[1] << 8;
        return res;
    }   

    inline uint32_t readLE32(const uint8_t* ptr) {
        uint32_t res;
        res =  (uint32_t)ptr[0];
        res |= (uint32_t)ptr[1] << 8;
        res |= (uint32_t)ptr[2] << 16;
        res |= (uint32_t)ptr[3] << 24;
        return res;
    }
}