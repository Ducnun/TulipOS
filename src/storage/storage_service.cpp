#include "services/storage_service.h"
#include <string.h>

void StorageService::initializeStorage() {
    auto hardware = _host.getAvailableStorage();

    for (StorageDescriptor& desc : hardware) {
        MBR mbr;
        if (!mbr.read(*desc.device)) {
            _host.print("No MBR found on " + std::string(desc.defaultPrefix) + "\n");
            continue;
        }

        // ONLY iterate through found partitions
        for (size_t i = 0; i < mbr.partitionCount(); i++) {
            const Partition& part = mbr.getPartition(i);
            
            if (part.type() == 0x0C || part.type() == 0x0B) {
                Volume* vol = new Volume(*desc.device, part);
                FAT32FileSystem* fs = new FAT32FileSystem(*vol);

                if (fs->mount()) {
                    char prefix[16];
                    // Name them SD0, SD1, etc based on the physical order
                    snprintf(prefix, sizeof(prefix), "%s%d", desc.defaultPrefix, (int)i);
                    _vfs.mount(strdup(prefix), fs);
                    
                    _host.print("Mounted " + std::string(prefix) + ":\n");
                } else {
                    delete fs;
                    delete vol;
                }
            }
        }
    }
}

void StorageService::ls(const char* path) {
    IDirIterator* it = _vfs.openDir(path ? path : ".");
    if (!it) {
        _host.print("Directory not found.\n");
        return;
    }

    DirectoryEntry entry;
    while (it->next(entry)) {
        // Use the host to format and output
        char line[64];
        snprintf(line, sizeof(line), "%-16s %8u\n", entry.name, entry.size);
        _host.print(std::string(line));
    }
    delete it;
}