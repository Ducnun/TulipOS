#pragma once

#include <cstdint>

class ISystem{
public:
    virtual void delay(uint32_t ms) = 0;
    virtual uint32_t millis(void) = 0;
    virtual void yield() = 0;
};