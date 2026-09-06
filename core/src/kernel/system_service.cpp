#include "kernel/system_service.h"

void SystemService::yield(){ 
    _host.service(); 
}

uint32_t SystemService::millis(){ 
    return _host.millis(); 
}

void SystemService::delay(uint32_t ms){ 
    _host.delay(ms); 
}