#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>

class Arena {
public:
    using Mark = size_t;
    Arena() : _buffer(nullptr), _capacity(0), _offset(0) {}

    void init(void* buffer, size_t size) {
        _buffer = (uint8_t*)buffer;
        _capacity = size;
        _offset = 0;
    }

    void* alloc(size_t size) {
        // Alignment handling
#ifdef ESP32
        size = (size + 3) & ~3;
#else
        size = (size + 7) & ~7;
#endif

        if (!_buffer || _offset + size > _capacity) return nullptr;

        void* ptr = &_buffer[_offset];
        _offset += size;
        return ptr;
    }

    void reset() { _offset = 0; }

    // Helpers for ManagedArena
    uint32_t getOffset() const { return (uint32_t)_offset; }
    size_t getRemaining() const { return _capacity - _offset; }
    void* getPointer(uint32_t offset) const { return (void*)(_buffer + offset); }
    bool contains(void* ptr) { return (ptr >= _buffer && ptr < (_buffer + _capacity)); }

private:
    uint8_t* _buffer;
    size_t _capacity;
    size_t _offset;
};