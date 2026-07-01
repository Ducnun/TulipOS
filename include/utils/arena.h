#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>

class Arena{
public:
    Arena(size_t size) : _capacity(size), _offset(0){
        _buffer = (uint8_t*)malloc(size);
    }

    ~Arena(){free(_buffer);}

    void* alloc(size_t size){
        size = (size + 1) & ~1;

        if (_offset + _capacity > _capacity){
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

private:
    uint8_t* _buffer;
    size_t _capacity;
    size_t _offset;
};