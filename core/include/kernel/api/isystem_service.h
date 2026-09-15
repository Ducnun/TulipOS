#pragma once

#include "kernel/api/itask_control.h"
#include "kernel/api/imemory_service.h"
#include "kernel/api/iinput_service.h"
#include "kernel/api/ioutput_service.h"
#include "kernel/api/iconsole.h"

class IKernel{
public:
    // System service
    virtual ITaskControl& taskControl() = 0;

    // Exposing other service APIs
    virtual IMemoryService& memory()  = 0;
    virtual IInputService&  input()   = 0;
    virtual IOutputService& output()  = 0;
    virtual IConsole&       console() = 0;
};