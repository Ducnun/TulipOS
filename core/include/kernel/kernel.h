#pragma once

#include "api/isystem_service.h"
#include "api/iinput_service.h"
#include "api/ioutput_service.h"
#include "memory_service.h"
#include "hal/ihost.h"

class Kernel : public ISystemService{
public:
    Kernel(IHost& host);

    // System function calls
    void yield();
    bool checkBreak();
    void delay(size_t ms);
    size_t millis(void);

    // Register services with host dependencies
    void registerInputService(IInputService& service) { _input = &service; }; 
    void registerOutputService(IOutputService& service) { _output = &service; }; 

    // Getters for system services
    IMemoryService& memory()    {return _memory; };
    IInputService&  input()     {return *_input; };
    IOutputService& output()    {return *_output; };

private:
    IHost& _host;
    IInputService* _input;
    IOutputService* _output;
    MemoryService _memory;
};