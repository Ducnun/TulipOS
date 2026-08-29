// Compiled only if esp32 env is chosen
#ifdef ESP32
    #include "host/esp32_definitions.h"
    #include "host/esp32_host.h"
    #include "system/shell.h"

    // Include your hardware/storage layers
    #include "devices/sd_card_spi.h"
    #include "storage/mbr/mbr.h"
    #include "storage/block/volume.h"

    // Include the Filesystem layers
    #include "storage/filesystems/fat32/fat32_filesystem.h"
    #include "storage/filesystems/vfs/idir_iterator.h"
    #include "storage/filesystems/vfs/ifile.h"

    PS2Keyboard keyboard(KB_DATA, KB_CLK);
    UARTTerminal terminal(UARTTERMINAL_SPEED);
    SDCardSPI sdDriver(SD_MOSI, SD_MISO, SD_SCK, SD_CS);
    ESP32Host host(keyboard, terminal);
    MBR mbr;
    Shell shell(host);

    void setup(){
        // Start IO and shell
        Serial.begin(115200);
        keyboard.begin();

        // 1. Initialize Hardware
        if (!sdDriver.begin()) {
            Serial.println("CRITICAL: SD Driver failed to initialize.");
            return;
        }

        // 2. Locate Partition via MBR
        if (!mbr.read(sdDriver)) {
            Serial.println("CRITICAL: Could not read MBR.");
            return;
        }
        
        // Grab the first partition (Index 0)
        auto part = mbr.partition(0);
        if (part.type() == 0) {
            Serial.println("CRITICAL: Partition 0 is empty/invalid.");
            return;
        }

        // 3. Create the Block Device (Volume) for this partition
        // This handles the LBA offset math (so block 0 is the start of the partition)
        Volume vol(sdDriver, part);

        // 4. Initialize the Filesystem
        FAT32FileSystem fs(vol);
        Serial.print("Mounting FAT32... ");
        if (!fs.mount()) {
            Serial.println("FAILED.");
            return;
        }
        Serial.println("SUCCESS.");

        // 5. TEST: Directory Listing using IDirIterator
        Serial.println("\n=== ROOT DIRECTORY LISTING ===");
        IDirIterator* it = fs.openDir("/");
        if (it) {
            DirectoryEntry vfsEntry;
            while (it->next(vfsEntry)) {
                if (vfsEntry.isDirectory) {
                    Serial.printf("[DIR]  %s\n", vfsEntry.name);
                } else {
                    Serial.printf("FILE   %-12s  SIZE: %u bytes\n", vfsEntry.name, vfsEntry.size);
                }
            }
            delete it; // Clean up the iterator
        } else {
            Serial.println("Failed to open Root Directory Iterator.");
        }

        // 6. TEST: Path Resolution and File Reading
        // This tests walking the tree and reading data
        const char* testFile = "/PRIMES.BAS";
        Serial.printf("\n=== READING FILE: %s ===\n", testFile);
        
        IFile* file = fs.openFile(testFile);
        if (file) {
            Serial.printf("File opened successfully. Total Size: %u bytes\n", file->size());
            Serial.println("--- START OF CONTENT ---");

            char buffer[64];
            while (!file->eof()) {
                size_t bytesRead = file->read((uint8_t*)buffer, sizeof(buffer) - 1);
                buffer[bytesRead] = '\0'; // Null terminate for Serial printing
                Serial.print(buffer);
            }
            
            Serial.println("\n--- END OF CONTENT ---");
            
            file->close();
            delete file; // Clean up the file object
        } else {
            Serial.printf("Failed to find/open %s\n", testFile);
        }

        // 7. TEST: Deep Path Resolution (Optional)
        // Make sure you have a folder called 'GAMES' with a file 'SNAKE.BAS' to test this
        const char* deepFile = "/BENCH/PI.BAS";
        IFile* df = fs.openFile(deepFile);
        if (df) {
            Serial.printf("\nSuccess! Deep path resolved. Size of %s is %u\n", deepFile, df->size());
            delete df;
        }

        Serial.println("\n--- ALL TESTS COMPLETE ---");

        host.begin();
        shell.runREPL();
    }

    void loop(){
        // Loop is handled by REPL
    }

// Compiled only when native env is chosen
#else
    #include "host/console_host.h"
    #include "system/shell.h"
    ConsoleHost host;

    // Start IO and shell
    int main(){
        Shell shell(host);
        shell.runREPL();
    }
#endif