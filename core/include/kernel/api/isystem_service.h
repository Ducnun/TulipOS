#pragma once

#include "kernel/api/imemory_service.h"
#include "kernel/api/iinput_service.h"
#include "kernel/api/ioutput_service.h"

class ISystemService{
public:
    // System service
    virtual void yield() = 0;
    virtual bool checkBreak() = 0;
    virtual void delay(size_t ms) = 0;
    virtual size_t millis(void) = 0;

    // Exposing other service APIs
    virtual IMemoryService& memory() = 0;
    virtual IInputService&  input()  = 0;
    virtual IOutputService& output() = 0;
};