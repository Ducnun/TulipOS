#include "esp32p4_host.h"
#include "esp_log.h"
#include "platform_config.h"
#include "kernel/kernel.h"
#include "kernel/task_control.h"
#include "kernel/output_service.h"
#include "kernel/memory_service.h"
#include "kernel/keyboard_input_service.h"
#include "kernel/storage_service.h"
#include "kernel/console.h"

static const char* TAG = "TulipOS";

void os_task(void* arg) {
    ESP_LOGI(TAG, "OS task started");

    // 1. Initialize P4 Hardware
    static PS2Keyboard keyboard(KB_DATA, KB_CLK);
    static UARTTerminal terminal(UART_TERMINAL_BAUD, UART_TERMINAL_TX, UART_TERMINAL_RX);
    static SDCardSPI sdDriver(SPI2_HOST, SD_MOSI, SD_MISO, SD_SCK, SD_CS);
    static MemoryManager memoryManager;
    static ESP32P4Host host(keyboard, terminal, sdDriver, memoryManager);
    
    // Create kernel and main services
    static Kernel kernel(host);
    static TaskControl taskControl(host);
    static MemoryService memoryService(host.memory());
    static VFS vfs;
    static StorageService storageService(vfs, memoryService);
    static KeyboardInputService kbService(host.keyboard(), taskControl);
    static OutputService outputService(host.terminal());
    static Console console(kbService, outputService);

    // Register core services to the kernel
    kernel.registerTaskControl(taskControl);
    kernel.registerMemoryService(memoryService);
    kernel.registerStorageService(storageService);
    kernel.registerInputService(kbService);
    kernel.registerOutputService(outputService);
    kernel.registerConsole(console);

    ESP_LOGI(TAG, "Starting kernel");
    kernel.begin();
}

extern "C" void app_main() {
    ESP_LOGI(TAG, "app_main started");

    // Create a task with 64KB of stack
    BaseType_t result = xTaskCreatePinnedToCore(os_task, "os_task", 131070, NULL, 5, NULL, 0);
    if (result != pdPASS) {
        ESP_LOGE(TAG, "Failed to create OS task");
    }
}