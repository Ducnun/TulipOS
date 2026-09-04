#include "esp32p4_host.h"
#include "esp_timer.h"
#include "mem_manager.h"
#include <unistd.h>

std::vector<StorageDescriptor> ESP32P4Host::getAvailableStorage(void){
    std::vector<StorageDescriptor> devices;
    // Provide the SD card hardware device
    devices.push_back({"SD", &_sdCard});
    return devices;
}

void ESP32P4Host::service(){
    static uint32_t calls = 0;

    // Only call vTaskDelay every 3000 calls
    calls++;
    if (calls > 3000) {
        vTaskDelay(1);      // Feeds the watchdog
        calls = 0;
    }
}

void ESP32P4Host::delay(uint32_t ms){
    vTaskDelay(pdMS_TO_TICKS(ms));
}

uint32_t ESP32P4Host::millis(){
    return (uint32_t)(esp_timer_get_time() / 1000);
}