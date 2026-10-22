#include "esp32p4_host.h"
#include "esp_timer.h"
#include "mem_manager.h"
#include <unistd.h>

void ESP32P4Host::begin(){
    ESP_LOGI("ESP32P4Host", "Initializing video terminal");
    if (!_terminal.begin()) ESP_LOGE("ESP32P4Host", "Terminal initialization failed");

    ESP_LOGI("ESP32P4Host", "Initializing PS/2 keyboard");
    _kb.begin();
    ESP_LOGI("ESP32P4Host", "PS/2 keyboard initialization complete");

    ESP_LOGI("ESP32P4Host", "Initializing SD card");
    if (!_sdCard.begin()){
        ESP_LOGE("ESP32P4Host", "SD card initialization failed");
        _sdCardInit = false;
    } else {
        _sdCardInit = true;
    }
    ESP_LOGI("ESP32P4Host", "Hardware initialization complete");
}

void ESP32P4Host::service() {
    static uint32_t count = 0;
    // Only yield every 3000 calls (tweak this number as needed)
    if (++count > 3000) {
        vTaskDelay(1);
        count = 0;
    }
}

void ESP32P4Host::debugLog(const char* message) {
    ESP_LOGE("TulipOS", "%s", message ? message : "(null)");
}

std::vector<StorageDescriptor> ESP32P4Host::getAvailableStorage(void){
    std::vector<StorageDescriptor> devices;
    // The host knows that on ESP32, we have an SD Card.
    if (_sdCardInit) {
        devices.push_back({"SD", &_sdCard});
    }
    return devices;
}

void ESP32P4Host::delay(uint32_t ms){
    vTaskDelay(pdMS_TO_TICKS(ms));
}

uint32_t ESP32P4Host::millis(){
    return (uint32_t)(esp_timer_get_time() / 1000);
}