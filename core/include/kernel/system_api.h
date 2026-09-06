#pragma once

#include "iconsole.h"
#include "istorage.h"
#include "isystem.h"

// Aggregate of all APIs defined by services at the kernel level
class ISystemAPI{
public:
    virtual IConsole& console() = 0;
    virtual IStorage& storage() = 0;
    virtual ISystem& system() = 0;
};
