#include "mem_manager.h"

MemoryManager::MemoryManager(){}

uint32_t MemoryManager::generateMask(MemoryType type){
    uint32_t caps = MALLOC_CAP_8BIT;
    switch(type){
        case MemoryType::System:{
            caps |= MALLOC_CAP_INTERNAL;
            break;
        }
        case MemoryType::Program:{
            caps |= MALLOC_CAP_SPIRAM;
            break;
        }
        case MemoryType::Transient:{
            caps |= MALLOC_CAP_INTERNAL;
            break;
        }
        case MemoryType::DMA:{
            caps |= MALLOC_CAP_DMA;
            break;
        }
        default:{
            caps |= MALLOC_CAP_DEFAULT;
            break;
        }
    }
    return caps;
}

void* MemoryManager::alloc(size_t size, MemoryType type){
    uint32_t mask = generateMask(type);
    return heap_caps_malloc(size, mask);
}

void MemoryManager::free(void* ptr){
    heap_caps_free(ptr);
}

size_t MemoryManager::getFree(MemoryType type){
    uint32_t mask = generateMask(type);
    return heap_caps_get_free_size(mask);
}

size_t MemoryManager::getTotal(MemoryType type){
    return heap_caps_get_total_size(MALLOC_CAP_8BIT); // Simplified example
}