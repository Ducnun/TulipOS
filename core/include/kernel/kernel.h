#pragma once

#include "hal/ihost.h"
#include "storage_service.h"
#include "memory_service.h"
#include "console_service.h"
#include "system_service.h"
// #include "system/shell.h"
#include "system_api.h"

class Kernel : public ISystemAPI{
public:
    Kernel(IHost& host) : 
        _host(host),
        _vfs(),
        // Pass Terminal and Keyboard to Console (host needed for yield)
        _console(host.getTerminal(), host.getKeyboard(), host),
        // Pass the raw memory allocator to the Service
        _memory(host.getMemory()),
        // Pass VFS to Storage
        _storage(_vfs),
        _system(host)
    {}

    void boot();

    // The Shell uses these getters
    ConsoleService& console() { return _console; }
    MemoryService&  memory()  { return _memory;  }
    StorageService& storage() { return _storage; }
    SystemService&  system()  { return _system;  }

private:
    IHost& _host;
    VFS _vfs;
    ConsoleService _console;
    MemoryService _memory;
    StorageService _storage;
    SystemService _system;
};