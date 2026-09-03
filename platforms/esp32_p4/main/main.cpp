#include "esp32p4_host.h"
#include "system/shell.h"
#include "services/storage_service.h"

#define KB_DATA 31
#define KB_CLK  34

#define UARTTERMINAL_SPEED 1000000
#define SD_MOSI 23 
#define SD_MISO 19
#define SD_SCK  18
#define SD_CS   5

void os_task(void* arg) {
    // 1. Initialize P4 Hardware
    static PS2Keyboard keyboard(KB_DATA, KB_CLK);
    static UARTTerminal terminal(115200);
    static SDCardSPI sdDriver(SD_MOSI, SD_MISO, SD_SCK, SD_CS);
    static ESP32P4Host host(keyboard, terminal, sdDriver);

    static VFS vfs;
    static StorageService storage(vfs, host);
    static Shell shell(host, storage);

    host.begin();
    host.print("Tulip OS Booting...\n");
    shell.runREPL();
}

extern "C" void app_main() {
    // Create a task with 64KB of stack
    xTaskCreatePinnedToCore(os_task, "os_task", 131070, NULL, 5, NULL, 0);
}