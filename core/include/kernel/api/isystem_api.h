#pragma once

#include "kernel/api/imemory_service.h"

class ISystemAPI{
public:
    virtual IMemoryService& memory() = 0;
};