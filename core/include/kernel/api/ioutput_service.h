#pragma once

#include "utils/span.h"

class IOutputService {
public:
    ~IOutputService() = default;

    virtual void print(util::span<char> txt) = 0;
    virtual void println(util::span<char> txt) = 0;
    virtual void clear() = 0;
    virtual void setColor(uint8_t fg, uint8_t bg) = 0;
};