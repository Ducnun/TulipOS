#pragma once

#include "hal/ihost.h"
#include "isystem.h"

class SystemService : public ISystem{
public:
    SystemService(IHost& host) : _host(host) {}

    void delay(uint32_t ms);
    void yield();
    uint32_t millis();

private:
    IHost& _host;
};