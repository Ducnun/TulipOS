#pragma once

#include <cstddef>
#include <cstring>

namespace util{
    template <typename T>
    class span{
    public:
        // Constructors
        span() : _ptr(nullptr), _size(0) {}
        span(T* ptr, size_t size) : _ptr(ptr), _size(size) {}
        template<size_t N>
        span(T (&arr)[N]) : _ptr(arr), _size(N) {}  // C style array constructor
        span(const span&) = default;
        span& operator=(const span&) = default;
    
        // Accessors
        T* data() const { return _ptr; };
        size_t size() const { return _size; };
        bool empty() const { return _size == 0; };
        T& operator[](size_t index) const { return *_ptr[index]; };
    
    private:
        T* _ptr;
        size_t _size;
    };
}