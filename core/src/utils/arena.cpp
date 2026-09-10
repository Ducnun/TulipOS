#include "utils/arena.h"

void* Arena::alloc(size_t size, size_t alignment){
    if (!_buffer || size == 0) return nullptr;
    if (alignment == 0 || alignment & ((alignment - 1) != 0)) return nullptr;

    // Calculate address and alignment offset
    const uintptr_t currentAddress = reinterpret_cast<uintptr_t>(_buffer + _offset);
    const size_t misalignment = static_cast<size_t>(currentAddress % alignment);
    const size_t padding = misalignment == 0 ? 0 : alignment - misalignment;

    if (padding > _capacity - _offset) return nullptr;

    const size_t alignedOffset = _offset + padding;
    if (size > _capacity - alignedOffset) return nullptr;

    void* result = _buffer + alignedOffset;
    _offset = alignedOffset + size;
    return result;
}