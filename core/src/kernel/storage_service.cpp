#include "kernel/storage_service.h"
#include "storage/mbr/mbr.h"
#include "kernel/storage_service.h"
#include "storage/block/volume.h"
#include "storage/filesystems/fat32/fat32_filesystem.h"
#include <string>

void StorageService::mountHardware(const std::vector<StorageDescriptor>& hardware) {
    int driveIndex = 0;

    for (const auto& desc : hardware) {
        if (!desc.device) continue;

        MBR mbr;
        if (!mbr.read(*desc.device)) continue;

        for (size_t i = 0; i < mbr.partitionCount(); i++) {
            const Partition& part = mbr.getPartition(i);
            
            if (part.type() == 0x0C || part.type() == 0x0B) {
                // Now auto works because volume.h is included
                auto* vol = new Volume(*desc.device, part);
                auto* fs = new FAT32FileSystem(*vol);

                if (fs->mount()) {
                    // Force string conversion so '+' works
                    std::string mPath = "/" + std::string(desc.defaultPrefix) + std::to_string(driveIndex++);
                    _vfs.mount(mPath.c_str(), fs);
                } else {
                    delete fs;
                    delete vol;
                }
            }
        }
    }
    _vfs.cd("/"); 
}

IFile* StorageService::open(const std::string& path) {
    // VFS handles the path resolution (relative vs absolute)
    return _vfs.open(path.c_str()); 
}

bool StorageService::exists(const std::string& path) {
    IFile* f = _vfs.open(path.c_str());
    if (f) {
        f->close();
        delete f;
        return true;
    }
    return false;
}

bool StorageService::cd(const std::string& path) {
    return _vfs.cd(path.c_str());
}

std::string StorageService::getCwd() const {
    return _vfs.getCwd();
}

std::vector<DirectoryEntry> StorageService::listDirectory(const std::string& path) {
    std::vector<DirectoryEntry> entries;
    IDirIterator* it = _vfs.openDir(path.c_str());
    
    if (it) {
        DirectoryEntry entry;
        while (it->next(entry)) {
            entries.push_back(entry);
        }
        delete it;
    }
    return entries;
}