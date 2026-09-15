#include "esp32p4_host.h"
#include "platform_config.h"
#include "kernel/kernel.h"
#include "kernel/task_control.h"
#include "kernel/output_service.h"
#include "kernel/memory_service.h"
#include "kernel/keyboard_input_service.h"
#include "kernel/storage_service.h"
#include "kernel/console.h"

void os_task(void* arg) {
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

    kernel.begin();
}

extern "C" void app_main() {
    // Create a task with 64KB of stack
    xTaskCreatePinnedToCore(os_task, "os_task", 131070, NULL, 5, NULL, 0);
}