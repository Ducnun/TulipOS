#include "kernel/kernel.h"

Kernel::Kernel(IHost& host) : 
    _host(host), 
    _memory(_host.getMemory()) 
{}

void Kernel::yield(){
    _host.service();
}

bool Kernel::checkBreak(){
    return _host.checkBreakRequested();
}

void Kernel::delay(size_t ms){
   _host.delay(ms);
}

size_t Kernel::millis(){
    return _host.millis();
}