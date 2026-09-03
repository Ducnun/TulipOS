#include "esp32_definitions.h"
#include "esp32_host.h"
#include "system/shell.h"
#include "services/storage_service.h"

// Create hardware objects
PS2Keyboard keyboard(KB_DATA, KB_CLK);
UARTTerminal terminal(UARTTERMINAL_SPEED);
SDCardSPI sdDriver(SD_MOSI, SD_MISO, SD_SCK, SD_CS);
ESP32Host host(keyboard, terminal, sdDriver);

// Create services
VFS vfs;
StorageService storage(vfs, host);

// Start core applications
Shell shell(host, storage);

void setup(){
    // Init hardware
    host.begin();

    // Start services
    storage.initializeStorage();

    // Bootup shell
    shell.runREPL();
}

void loop(){
    // Loop is handled by REPL
}