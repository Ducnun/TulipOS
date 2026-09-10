#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <new>
#include <utility>

class Arena {
public:
    using Mark = size_t;
    Arena() : _buffer(nullptr), _capacity(0), _offset(0) {}

    void init(void* buffer, size_t size) {
        _buffer = (std::byte*)buffer;
        _capacity = size;
        _offset = 0;
    }

    void* alloc(size_t size, size_t alignment = alignof(std::max_align_t));

    // For temporary storage
    Mark mark() const { return _offset; };
    bool rewind(Mark mark){
        if (mark > _offset) return false;
        _offset = mark;
        return true;
    }

    void reset() { _offset = 0; }

    // Helpers for ManagedArena
    size_t getOffset() const { return _offset; }
    size_t getCapacity() const { return _capacity; }
    size_t getRemaining() const { return _capacity - _offset; }

    void* getPointer(size_t offset) {
        if (!_buffer || offset > _capacity) return nullptr;
        return _buffer + offset;
    }

    const void* getPointer(size_t offset) const {
        if (!_buffer || offset > _capacity) return nullptr;
        return _buffer + offset;
    }

    bool contains(const void* pointer) const {
        if (!_buffer || !pointer) return false;

        const uintptr_t address = reinterpret_cast<uintptr_t>(pointer);
        const uintptr_t begin = reinterpret_cast<uintptr_t>(_buffer);
        const uintptr_t end = begin + _capacity;

        return address >= begin && address < end;
    }

    // Allocation templates
    template<typename T>
    T* allocArray(size_t count = 1) {
        if (count == 0) {
            return nullptr;
        }

        if (count >
            std::numeric_limits<size_t>::max() / sizeof(T)) {
            return nullptr;
        }

        return static_cast<T*>(
            alloc(sizeof(T) * count, alignof(T))
        );
    }

    template<typename T, typename... Args>
    T* create(Args&&... args) {
        void* memory = alloc(sizeof(T), alignof(T));

        if (!memory) {
            return nullptr;
        }

        return new (memory) T(
            std::forward<Args>(args)...
        );
    }

private:
    std::byte* _buffer;
    size_t _capacity;
    size_t _offset;
};