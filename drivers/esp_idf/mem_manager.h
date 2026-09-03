#pragma once

#include "hal/imemory.h"
#include "esp_heap_caps.h"

class MemoryManager : public IMemoryManager{
public:
    MemoryManager();

    void* alloc(size_t size, MemoryType type);
    void free(void* ptr);

    size_t getFree(MemoryType type);
    size_t getTotal(MemoryType type);

private:
    uint32_t generateMask(MemoryType type);
};