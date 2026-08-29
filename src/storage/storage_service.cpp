#include "services/storage_service.h"
#include <string.h>

void StorageService::initializeStorage() {
    // Ask host for all physical disks
    std::vector<StorageDescriptor> hardware = _host.getAvailableStorage();

    for (StorageDescriptor& desc : hardware) {
        MBR mbr;
        if (!mbr.read(*desc.device)) continue;

        // Iterate through partitions found on disk
        for (int i = 0; i < 4; i++) {
            Partition part = mbr.partition(i);
            if (part.type() == 0x0C || part.type() == 0x0B) {
                // Wrap partition in Volume logic
                Volume* vol = new Volume(*desc.device, part);
                FAT32FileSystem* fs = new FAT32FileSystem(*vol);

                // Mount the concrete file system, then mount that into the VFS
                if (fs->mount()) {
                    char prefix[16];
                    sprintf(prefix, "%s%d", desc.defaultPrefix, i);
                    _vfs.mount(strdup(prefix), fs);
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