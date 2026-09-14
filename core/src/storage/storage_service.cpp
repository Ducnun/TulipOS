// #include "services/storage_service.h"
// #include <string.h>

// void StorageService::initializeStorage() {
//     auto hardware = _host.getAvailableStorage();

//     for (StorageDescriptor& desc : hardware) {
//         MBR mbr;
//         if (!mbr.read(*desc.device)) {
//             _host.print("No MBR found on " + std::string(desc.defaultPrefix) + "\n");
//             continue;
//         }

//         // ONLY iterate through found partitions
//         for (size_t i = 0; i < mbr.partitionCount(); i++) {
//             const Partition& part = mbr.getPartition(i);
            
//             if (part.type() == 0x0C || part.type() == 0x0B) {
//                 Volume* vol = new Volume(*desc.device, part);
//                 FAT32FileSystem* fs = new FAT32FileSystem(*vol);

//                 if (fs->mount()) {
//                     char prefix[16];
//                     // Name them SD0, SD1, etc based on the physical order
//                     snprintf(prefix, sizeof(prefix), "%s%d", desc.defaultPrefix, (int)i);
//                     _vfs.mount(strdup(prefix), fs);
                    
//                     _host.print("Mounted " + std::string(prefix) + ":\n");
//                 } else {
//                     delete fs;
//                     delete vol;
//                 }
//             }
//         }
//     }

//     if (_vfs.getMountCount() > 0) {
//         // Automatically "log in" to the first drive found
//         _vfs.setActiveDrive(0); 
//         _host.print("System: Boot drive is " + std::string(_vfs.getActiveDrivePrefix()) + ":\n");
//     }
// }

// IFile* StorageService::open(const char* path) {
//     if (!path || strlen(path) == 0) {
//         _host.print("? ERROR: No filename provided\n");
//         return nullptr;
//     }

//     // Pass the request to the VFS. 
//     IFile* file = _vfs.open(path);

//     if (!file) {
//         // If the file wasn't found, we report it here so the Shell doesn't have to.
//         char errorBuf[64];
//         snprintf(errorBuf, sizeof(errorBuf), "? ERROR: Could not open '%s'\n", path);
//         _host.print(errorBuf);
//     }

//     return file;
// }

// void StorageService::ls(const char* path) {
//     // Standardize path to "." if none provided
//     IDirIterator* it = _vfs.openDir(path ? path : ".");
    
//     if (!it) {
//         _host.print("? ERROR: Directory not found\n");
//         return;
//     }

//     // Print Header
//     _host.print("\nName             Size        Type\n");
//     _host.print("------------------------------------\n");

//     DirectoryEntry entry;
//     uint32_t count = 0;
//     uint32_t totalSize = 0;

//     while (it->next(entry)) {
//         char sizeBuf[16];
//         const char* typeStr;

//         if (entry.isDirectory) {
//             strncpy(sizeBuf, "    ---", sizeof(sizeBuf));
//             typeStr = "<DIR>";
//         } else {
//             formatSize(entry.size, sizeBuf, sizeof(sizeBuf));
//             typeStr = "FILE";
//             totalSize += entry.size;
//         }

//         // Formatting string explanation:
//         // %-16s -> Name, left-aligned, 16 chars
//         // %10s  -> Size, right-aligned, 10 chars
//         // %7s   -> Type, right-aligned, 7 chars
//         char line[80];
//         snprintf(line, sizeof(line), "%-16s %10s  %7s\n", 
//                  entry.name, 
//                  sizeBuf, 
//                  typeStr);
        
//         _host.print(std::string(line));
//         count++;
//     }

//     // Print Footer
//     _host.print("------------------------------------\n");
    
//     char totalSizeBuf[16];
//     formatSize(totalSize, totalSizeBuf, sizeof(totalSizeBuf));
    
//     char footer[64];
//     snprintf(footer, sizeof(footer), "%u Object(s) found. (%s total)\n\n", (unsigned int)count, totalSizeBuf);
//     _host.print(std::string(footer));

//     delete it;
// }

// bool StorageService::cd(const char* path) {
//     if (!path || strlen(path) == 0) {
//         _host.print("? ERROR: Path required\n");
//         return false;
//     }

//     if (_vfs.cd(path)) {
//         // Success! No message needed, the next prompt will show the new path
//         return true;
//     } else {
//         _host.print("? ERROR: Path not found\n");
//         return false;
//     }
// }

// void StorageService::formatSize(uint32_t bytes, char* out, size_t outSize) {
//     if (bytes < 1024) {
//         // Bytes: 512 B
//         snprintf(out, outSize, "%u B", (unsigned int)bytes);
//     } else if (bytes < 1024 * 1024) {
//         // Kilobytes: 12.5 KB
//         snprintf(out, outSize, "%.1f KB", (float)bytes / 1024.0f);
//     } else {
//         // Megabytes: 1.4 MB
//         snprintf(out, outSize, "%.1f MB", (float)bytes / (1024.0f * 1024.0f));
//     }
// }