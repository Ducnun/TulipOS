#include "esp32p4_host.h"
#include "system/shell.h"
#include "services/storage_service.h"
#include "platform_config.h"
#include "kernel/kernel.h"
#include "kernel/output_service.h"
#include "kernel/keyboard_input_service.h"
#include "kernel/console.h"

void os_task(void* arg) {
    // 1. Initialize P4 Hardware
    static PS2Keyboard keyboard(KB_DATA, KB_CLK);
    static UARTTerminal terminal(UART_TERMINAL_BAUD, UART_TERMINAL_TX, UART_TERMINAL_RX);
    static SDCardSPI sdDriver(SD_MOSI, SD_MISO, SD_SCK, SD_CS);
    static MemoryManager memory;
    static ESP32P4Host host(keyboard, terminal, sdDriver, memory);
    
    // Kernel and service registration
    static Kernel kernel(host);
    static KeyboardInputService kbService(host.keyboard(), kernel);
    static OutputService outputService(host.terminal());
    static Console console(kbService, outputService);
    kernel.registerInputService(kbService);
    kernel.registerOutputService(outputService);
    kernel.registerConsole(console);
    kernel.begin();
    
    // static VFS vfs;
    // static StorageService storage(vfs, host);
    // static Shell shell(host, storage);

    kernel.output().println({"Tulip OS Booting..."});
}

extern "C" void app_main() {
    // Create a task with 64KB of stack
    xTaskCreatePinnedToCore(os_task, "os_task", 131070, NULL, 5, NULL, 0);
}