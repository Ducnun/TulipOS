#pragma once

#include <stddef.h>

class ITaskControl{
public:
    ~ITaskControl() = default;
    virtual void yield() = 0;
    virtual void delay(size_t ms) = 0;
    virtual size_t millis(void) = 0;
    virtual bool checkBreakRequested(void) = 0;
    virtual void setBreakRequested(bool requested) = 0;
};
