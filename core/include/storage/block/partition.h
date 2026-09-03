#pragma once

#include <cstdint>

class Partition{
public:
    Partition(uint32_t start, uint32_t size, uint8_t type) : _start(start), _size(size), _type(type) {}
    uint32_t startLBA() const { return _start; }
    uint32_t sectorCount() const { return _size; }
    uint8_t type() const { return _type; }

private:
    uint32_t _start;
    uint32_t _size;
    uint8_t _type;
};