#include "esp32p4_host.h"
#include "kernel/kernel.h"
#include "system/shell.h"
#include "platform_config.h"

void os_task(void* arg) {
    // 1. Initialize P4 Hardware and platform host
    static SDCardSPI sdDriver(SD_MOSI, SD_MISO, SD_SCK, SD_CS);
    static MemoryManager memory;
    static UARTTerminal terminal(UART_TERMINAL_BAUD, UART_TERMINAL_TX, UART_TERMINAL_RX);
    static PS2Keyboard keyboard(KB_DATA, KB_CLK);
    static ESP32P4Host host(sdDriver, memory, terminal, keyboard);

    // Create and boot kernel
    static Kernel kernel(host);
    kernel.boot();
}

extern "C" void app_main() {
    // Create a task with 64KB of stack
    xTaskCreatePinnedToCore(os_task, "os_task", 131070, NULL, 5, NULL, 0);
}