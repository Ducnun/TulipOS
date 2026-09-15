#include "kernel/task_control.h"

void TaskControl::yield(){
    _host.service();
}

void TaskControl::delay(size_t ms){
    _host.delay(ms);
}

size_t TaskControl::millis(void){
    return _host.millis();
}
