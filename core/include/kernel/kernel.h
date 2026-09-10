#pragma once

#include "api/isystem_api.h"
#include "memory_service.h"
#include "hal/ihost.h"

class Kernel : public ISystemAPI{
public:
    Kernel(IHost& _host);

    IMemoryService& memory() {return _memory; };
    
private:
    IHost& _host;
    MemoryService _memory;
};