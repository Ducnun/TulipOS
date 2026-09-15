#pragma once

#include "hal/ihost.h"
#include "api/isystem_service.h"
#include "api/iinput_service.h"
#include "api/ioutput_service.h"
#include "api/iconsole.h"
#include "api/itask_control.h"
#include "api/istorage_service.h"
#include "memory_service.h"

class Kernel : public IKernel{
public:
    Kernel(IHost& host);

    void begin();

    // Register services with host dependencies
    void registerTaskControl(ITaskControl& taskControl)     { _taskControl = &taskControl; };
    void registerInputService(IInputService& service)       { _input = &service; }; 
    void registerOutputService(IOutputService& service)     { _output = &service; }; 
    void registerMemoryService(IMemoryService& service)     { _memory = &service; }; 
    void registerStorageService(IStorageService& service)   {_storage = &service; };
    void registerConsole(IConsole& console)                 { _console = &console; };

    // Getters for system services
    ITaskControl&   taskControl()   {return *_taskControl; };
    IMemoryService& memory()        {return *_memory; };
    IInputService&  input()         {return *_input; };
    IOutputService& output()        {return *_output; };
    IConsole&       console()       {return *_console; };
    IStorageService& storage()      {return *_storage; };

private:
    IHost& _host;
    IMemoryService* _memory;
    ITaskControl* _taskControl;
    IStorageService* _storage;
    IInputService* _input;
    IOutputService* _output;
    IConsole* _console;

    // Bootup helpers
    void initStorage();
};