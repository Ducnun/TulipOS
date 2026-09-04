#pragma once

#include "hal/imemory.h"

class MemoryService{
public:
    MemoryService(IMemoryManager* memory) : _memory(memory) {}

    void* alloc(size_t size, MemoryType type);
    void free(void* ptr, size_t, MemoryType type);

    size_t getFree(MemoryType type) { return _memory->getFree(type); }
    size_t getProgramAllocated() { return _programAllocated; }
    size_t getSystemAllocated() {return _systemAllocated; }
private:
    IMemoryManager* _memory;
    size_t _programAllocated;
    size_t _systemAllocated;
};
