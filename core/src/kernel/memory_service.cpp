#include "kernel/memory_service.h"

void* MemoryService::alloc(size_t size, MemoryType type){
    // Prevent too large allocations
    if (size == 0 || size > 4 * 1024 * 1024) return nullptr; 
    // Prevent unnecessarily large transient memory allocations
    if (type == MemoryType::Transient && size > 1024 * 64) return nullptr;

    void* ptr = _memory.alloc(size, type);
    if (ptr) {
        if (type == MemoryType::System || type == MemoryType::DMA) {
            _systemAllocated += size;
        } else {
            _programAllocated += size;
        }
    }
    return ptr;
}

void MemoryService::free(void* ptr, size_t size, MemoryType type) {
    if (!ptr) return;

    _memory.free(ptr);
    if (type == MemoryType::System || type == MemoryType::DMA) {
        _systemAllocated = size <= _systemAllocated ? _systemAllocated - size : 0;
    } else {
        _programAllocated = size <= _programAllocated ? _programAllocated - size : 0;
    }
}