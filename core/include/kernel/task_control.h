#pragma once

#include "api/itask_control.h"
#include "hal/ihost.h"

class TaskControl : public ITaskControl{
public:
    TaskControl(IHost& host) : _host(host) {}
    void yield();
    void delay(size_t ms);
    size_t millis(void);
    bool checkBreakRequested(void) { return _breakRequested; }
    void setBreakRequested(bool requested) { _breakRequested = requested; }

private:
    IHost& _host;
    bool _breakRequested = false;
};