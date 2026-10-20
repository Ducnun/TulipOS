#pragma once

#include "hal/ihost.h"
#include "api/isystem_service.h"
#include "api/iinput_service.h"
#include "api/ioutput_service.h"
#include "api/itask_control.h"
#include "api/istorage_service.h"
#include "system/api/iconsole.h"
#include "memory_service.h"
#include "system/console.h"
#include "system/console_program_io.h"
#include "system/shell.h"
#include <memory>

class Kernel : public IKernel{
public:
    Kernel(IHost& host);

    bool begin();

    // Register services with host dependencies
    void registerTaskControl(ITaskControl& taskControl)     { _taskControl = &taskControl; };
    void registerInputService(IInputService& service)       { _input = &service; }; 
    void registerOutputService(IOutputService& service)     { _output = &service; }; 
    void registerMemoryService(IMemoryService& service)     { _memory = &service; }; 
    void registerStorageService(IStorageService& service)   {_storage = &service; };

    // Getters for system services
    ITaskControl& taskControl() override { return *_taskControl; }
    IMemoryService& memory() override { return *_memory; }
    IInputService& input() override { return *_input; }
    IOutputService& output() override { return *_output; }
    IConsole& console() override { return *_console; }
    IStorageService& storage() { return *_storage; }

private:
    IHost& _host;
    IMemoryService* _memory = nullptr;
    ITaskControl* _taskControl = nullptr;
    IStorageService* _storage = nullptr;
    IInputService* _input = nullptr;
    IOutputService* _output = nullptr;
    std::unique_ptr<Console> _console;
    std::unique_ptr<ConsoleProgramIO> _consoleProgramIO;
    std::unique_ptr<Shell> _shell;

    // Bootup helpers
    bool initStorage();
};