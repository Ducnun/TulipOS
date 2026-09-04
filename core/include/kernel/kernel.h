#pragma once

#include "hal/ihost.h"
#include "storage_service.h"
#include "memory_service.h"
#include "console_service.h"
#include "system/shell.h"

class Kernel {
public:
    Kernel(IHost& host) : 
        _host(host),
        _vfs(),
        // Pass Terminal and Keyboard to Console
        _console(host.getTerminal(), host.getKeyboard(), host),
        // Pass the raw memory allocator to the Service
        _memory(host.getMemory()),
        // Pass VFS to Storage
        _storage(_vfs) 
    {}

    void boot();

    // The Shell uses these getters
    ConsoleService& console() { return _console; }
    MemoryService&  memory()  { return _memory;  }
    StorageService& storage() { return _storage; }

    // Use host directly for platform utilities
    void yield() { _host.service(); }
    uint32_t millis() { return _host.millis(); }
    void delay(uint32_t ms) { _host.delay(ms); }

private:
    IHost& _host;
    VFS _vfs;
    ConsoleService _console;
    MemoryService _memory;
    StorageService _storage;
};