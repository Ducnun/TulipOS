#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>

class Arena{
public:
    Arena() : _buffer(nullptr), _capacity(0), _offset(0) {}

    // Initializer used by the Kernel/Shell
    void init(void* buffer, size_t size) {
        _buffer = (uint8_t*)buffer;
        _capacity = size;
        _offset = 0;
    }

    ~Arena(){free(_buffer);}

    void* alloc(size_t size){
        #ifdef ESP32
        size = (size + 3) & ~3;
        #else
        size = (size + 7) & ~7;
        #endif

        // Not necessarily an issue if triggered in a managed environment
        // (e.g. triggering a swap in managed_arena.h)
        if (_offset + size > _capacity){
            return nullptr;
        }

        void *ptr = &_buffer[_offset];
        _offset += size;

        return ptr;
    }

    void reset(){
        _offset = 0;
    }

    const char* dupString(const char* s) {
        size_t len = strlen(s) + 1;
        char* dest = (char*)alloc(len);
        if (dest) memcpy(dest, s, len);
        return dest;
    }

    uint32_t getOffset() const { return (uint32_t)_offset; }
    void* getPointer(uint32_t offset) const { return (void*)(_buffer + offset) ;}
    bool contains(void* ptr) { return (ptr >= _buffer && ptr < (_buffer + _capacity)); }

private:
    uint8_t* _buffer;
    size_t _capacity;
    size_t _offset;
};