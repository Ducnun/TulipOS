#pragma once

#include <stdint.h>

namespace util{
    inline uint16_t readLE16(const uint8_t* ptr){
        return  static_cast<uint16_t>(ptr[0]) | 
                (static_cast<uint16_t>(ptr[1] << 8));
    }   

    inline uint32_t readLE32(const uint8_t* ptr){
        return  static_cast<uint32_t>(ptr[0]) | 
                (static_cast<uint32_t>(ptr[1] << 8)) |
                (static_cast<uint32_t>(ptr[1] << 16)) |
                (static_cast<uint32_t>(ptr[1] << 24));
    }
}