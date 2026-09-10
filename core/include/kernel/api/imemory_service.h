#pragma once

#include "hal/imemory.h"

class IMemoryService{
public:
    virtual ~IMemoryService() = default;

    virtual void* alloc(size_t size, MemoryType type) = 0;
    virtual void free(void* ptr, size_t size, MemoryType type) = 0;
    virtual size_t getFree(MemoryType type) = 0;
    virtual size_t getProgramAllocated() = 0 ;
    virtual size_t getSystemAllocated() = 0 ;
};
