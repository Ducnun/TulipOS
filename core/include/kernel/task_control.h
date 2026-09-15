#pragma once

#include "api/itask_control.h"
#include "hal/ihost.h"

class TaskControl : public ITaskControl{
public:
    TaskControl(IHost& host) : _host(host) {}
    void yield();
    void delay(size_t ms);
    size_t millis(void);

private:
    IHost& _host;
};