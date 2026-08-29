// Compiled only if esp32 env is chosen
#ifdef ESP32
  #include "host/esp32_definitions.h"
  #include "host/esp32_host.h"
  #include "system/shell.h"
  #include "devices/sd_card_spi.h"
  #include "storage/mbr/mbr.h"
  #include "storage/block/volume.h"
  #include "storage/filesystems/fat32/fat32_bpb.h"
  #include "storage/filesystems/fat32/fat32_clustertable.h"
  #include "storage/filesystems/fat32/fat32_directory.h"
  #include "storage/filesystems/fat32/fat32_file.h"

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

    // SD CARD/FileFormat testing
    sdDriver.begin();

    // Debug MBR reading
    Serial.println("=== DEBUG: Reading MBR ===");
    if (!mbr.read(sdDriver)) {
        Serial.println("ERROR: Failed to read MBR!");
        while(1); // Halt
    }
    Serial.println("MBR read successfully");

    // Debug partition info
    auto partition = mbr.partition(0);
    Serial.println("\n=== DEBUG: Partition Info ===");
    Serial.print("Partition type: 0x");
    Serial.println(partition.type(), HEX);
    Serial.print("Partition starts at LBA: ");
    Serial.println(partition.startLBA());  // Should be 2048
    Serial.print("Partition size (sectors): ");
    Serial.println(partition.sectorCount());

    // Debug Volume creation
    Serial.println("\n=== DEBUG: Creating Volume ===");
    Volume volume(sdDriver, partition);
    Serial.println("Volume created successfully");

    // Debug raw sector read (bypass Volume class)
    Serial.println("\n=== DEBUG: Raw Sector Read (LBA 2048) ===");
    uint8_t rawBoot[512];
    if (!sdDriver.readBlock(partition.startLBA(), rawBoot)) {
        Serial.println("ERROR: Failed to read raw sector!");
    } else {
        Serial.println("Raw sector read successful");
        Serial.println("First 16 bytes (hex):");
        for (int i = 0; i < 16; i++) {
            if (rawBoot[i] < 0x10) Serial.print("0");
            Serial.print(rawBoot[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }

    // Debug Volume read
    Serial.println("\n=== DEBUG: Volume Sector Read ===");
    uint8_t boot[512];
    if (!volume.readBlock(0, boot)) {  // Read block 0 (should be LBA 2048)
        Serial.println("ERROR: Failed to read via Volume!");
    } else {
        Serial.println("Volume sector read successful");
        Serial.println("First 32 bytes of boot sector (hex):");
        for (int i = 0; i < 32; i++) {
            if (boot[i] < 0x10) Serial.print("0");
            Serial.print(boot[i], HEX);
            Serial.print(" ");
            if ((i + 1) % 16 == 0) Serial.println();
        }
        Serial.println();

        // Print last 2 bytes (should be 0x55 0xAA)
        Serial.print("Last 2 bytes (signature): 0x");
        Serial.print(boot[510], HEX);
        Serial.print(boot[511], HEX);
        Serial.println();

        // Debug BPB parsing
        Serial.println("\n=== DEBUG: BPB Parsing ===");
        FAT32BPB bpb;
        if (bpb.parse(boot)) {
            Serial.println("FAT32 BPB parsed successfully");
            Serial.print("Bytes per sector: ");
            Serial.println(bpb.bytesPerSector());
            Serial.print("Sectors per cluster: ");
            Serial.println(bpb.sectorsPerCluster());
            Serial.print("Reserved sectors: ");
            Serial.println(bpb.reservedSectors());
            Serial.print("Number of FATs: ");
            Serial.println(bpb.numberOfFats());
            Serial.print("FAT size (sectors): ");
            Serial.println(bpb.fatSize());
            Serial.print("Root cluster: ");
            Serial.println(bpb.rootCluster());
        } else {
            Serial.println("Failed to parse FAT32 BPB");
            Serial.print("First 3 bytes: 0x");
            Serial.print(boot[0], HEX);
            Serial.print(boot[1], HEX);
            Serial.println(boot[2], HEX);
            Serial.print("Signature: 0x");
            Serial.print(boot[510], HEX);
            Serial.println(boot[511], HEX);
        }

        // Test out root directory clusters
        FAT32ClusterTable clusters(volume, bpb);

        uint32_t cluster = bpb.rootCluster();

        Serial.println("\n=== DEBUG: Root Directory Cluster Chain ===");

        while (true) {
            Serial.print("Cluster: ");
            Serial.println(cluster);

            uint32_t firstSector = clusters.firstSector(cluster);

            Serial.print("  First sector: ");
            Serial.println(firstSector);

            uint32_t next = clusters.nextCluster(cluster);

            Serial.print("  Next cluster: ");
            Serial.println(next, HEX);

            if (clusters.isEndOfChain(next)) {
                Serial.println("  END OF CHAIN");
                break;
            }

            cluster = next;
        }

        Serial.println("\n=== DEBUG: Directory Listing (Root) ===");

        // 1. Initialize the Directory iterator
        FAT32Directory rootDir(volume, clusters, bpb.rootCluster());
        FAT32DirectoryEntry entry;

        int count = 0;
        while (rootDir.next(entry)) {
            count++;
            
            // 2. Determine if it's a directory or a file
            bool isDir = (entry.attributes & FAT_ATTR_DIRECTORY);
            
            // 3. Print the name and extension
            // Note: FAT pads with spaces, so we'll see things like "MYFILE  TXT"
            Serial.print(isDir ? "[DIR] " : "      ");
            Serial.print(entry.name);
            Serial.print(".");
            Serial.print(entry.extension);

            if (strcmp(entry.name, "PRIMES  ") == 0) {
                Serial.println("\n--- Content of PRIMES.BAS ---");
                
                FAT32File file(volume, clusters, entry);
                char lineBuffer[64];
                
                while (!file.eof()) {
                    size_t n = file.read((uint8_t*)lineBuffer, sizeof(lineBuffer) - 1);
                    lineBuffer[n] = '\0'; // Null terminate for printing
                    Serial.print(lineBuffer);
                }
                Serial.println("\n--- End of File ---");
            }


            // 4. Print file size (directories usually show 0)
            if (!isDir) {
                Serial.print("  Size: ");
                Serial.print(entry.fileSize);
                Serial.println(" bytes");
            } else {
                Serial.println();
            }

            // 5. Print the starting cluster (useful for verifying your ClusterTable)
            Serial.print("        Start Cluster: ");
            Serial.println(entry.firstCluster);
        }

        if (count == 0) {
            Serial.println("No files found in root directory.");
        } else {
            Serial.printf("\nTotal entries found: %d\n", count);
        }
    }

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