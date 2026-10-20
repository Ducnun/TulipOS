#pragma once

#include <cstddef>
#include "utils/stringview.h"

class IConsole {
public:
    virtual ~IConsole() = default;
    virtual bool readLine(util::StringView prompt, char* buffer, size_t capacity) = 0;
    virtual void clear() = 0;
};