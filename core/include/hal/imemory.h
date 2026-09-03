#pragma once

#include <stddef.h>

enum class MemoryType {
    Transient,   // Short-lived: Interpreter temp variables, scratchpads
    Program,     // Long-lived: The actual BASIC source code storage
    System,      // OS Core: Task stacks, device drivers, lookup tables
    DMA,         // Hardware: Buffers for SPI/UART/SD Card (must be internal)
};

class IMemoryManager {
public:
    virtual ~IMemoryManager() = default;
    
    virtual void* alloc(size_t size, MemoryType type) = 0;
    virtual void free(void* ptr) = 0;
    
    // Useful for the BASIC 'FRE(0)' command
    virtual size_t getFree(MemoryType type) = 0;
    virtual size_t getTotal(MemoryType type) = 0;
};