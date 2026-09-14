#pragma once

#include "hal/ihost.h"
#include "api/isystem_service.h"
#include "api/iinput_service.h"
#include "api/ioutput_service.h"
#include "api/iconsole.h"
#include "memory_service.h"

class Kernel : public ISystemService{
public:
    Kernel(IHost& host);

    void begin();

    // System function calls
    void yield();
    bool checkBreak();
    void delay(size_t ms);
    size_t millis(void);

    // Register services with host dependencies
    void registerInputService(IInputService& service) { _input = &service; }; 
    void registerOutputService(IOutputService& service) { _output = &service; }; 
    void registerMemoryService(IMemoryService& service) { _memory = &service; }; 
    void registerConsole(IConsole& console) { _console = &console; };

    // Getters for system services
    IMemoryService& memory()    {return *_memory; };
    IInputService&  input()     {return *_input; };
    IOutputService& output()    {return *_output; };
    IConsole&       console()   {return *_console;};

private:
    IHost& _host;
    IInputService* _input;
    IOutputService* _output;
    IConsole* _console;
    IMemoryService* _memory;
};