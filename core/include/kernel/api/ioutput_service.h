#pragma once

#include "utils/stringview.h"

class IOutputService {
public:
    ~IOutputService() = default;

    virtual void print(util::StringView txt) = 0;
    virtual void print(char c) = 0;
    virtual void println(util::StringView txt) = 0;
    virtual void clear() = 0;
    virtual void setColor(uint8_t fg, uint8_t bg) = 0;
};