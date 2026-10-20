#pragma once

#include <cstddef>
#include "utils/stringview.h"

class IProgramIO{
public:
    virtual ~IProgramIO() = default;

    virtual void write(util::StringView data) = 0;
    virtual bool readLine(char* buffer, size_t capacity) = 0;
};