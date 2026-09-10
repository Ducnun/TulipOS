#include "kernel/kernel.h"

Kernel::Kernel(IHost& host) : 
    _host(host), 
    _memory(_host.getMemory()) 
{}