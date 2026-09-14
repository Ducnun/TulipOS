#pragma once

#include "utils/span.h"
#include "utils/stringview.h"

class IConsole {
public:
    virtual ~IConsole() = default;
    virtual void run() = 0;
};