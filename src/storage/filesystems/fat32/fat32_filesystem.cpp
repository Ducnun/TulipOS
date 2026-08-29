#include "storage/filesystems/fat32/fat32_filesystem.h"

#include <string.h>
#include <strings.h>

static bool matches(const FAT32DirectoryEntry& entry, const char* searchName) {
    char entryName[13]; // 8 + 1 + 3 + null
    int p = 0;

    // Copy name part (strip trailing spaces)
    for (int i = 0; i < 8; i++) {
        if (entry.name[i] == ' ') break;
        entryName[p++] = entry.name[i];
    }

    // Add dot if there is an extension
    if (entry.extension[0] != ' ') {
        entryName[p++] = '.';
        // Copy extension part (strip trailing spaces)
        for (int i = 0; i < 3; i++) {
            if (entry.extension[i] == ' ') break;
            entryName[p++] = entry.extension[i];
        }
    }
    
    entryName[p] = '\0';

    // FAT is case-insensitive, so we use strcasecmp
    return strcasecmp(entryName, searchName) == 0;
}

bool FAT32FileSystem::mount() {
    // Clean up old cluster table if it exists (Memory Safety)
    if (_clusters != nullptr) {
        delete _clusters;
        _clusters = nullptr;
    }

    // Read the first sector of the partition (the Boot Sector / BPB)
    uint8_t sectorBuffer[512];
    if (!_dev.readBlock(0, sectorBuffer)) {
        return false;
    }

    // Parse the BPB using the data we just read
    if (!_bpb.parse(sectorBuffer)) {
        return false;
    }

    // Double-check that this is actually a FAT32 volume
    if (_bpb.fatSize() == 0) {
        return false; 
    }

    // Initialize the Cluster Table
    _clusters = new FAT32ClusterTable(_dev, _bpb);
    
    return (_clusters != nullptr);
}

bool FAT32FileSystem::resolvePath(const char* path, FAT32DirectoryEntry& outEntry) {
    // Standardize: if path is empty or "/", it's the root (no entry to return)
    if (!path || path[0] == '\0' || strcmp(path, "/") == 0) return false;

    uint32_t currentDirCluster = _bpb.rootCluster();
    char buffer[256];
    strncpy(buffer, path, sizeof(buffer));
    
    char* part = strtok(buffer, "/");
    while (part != nullptr) {
        // Skip empty parts or "." parts
        if (strcmp(part, ".") == 0) {
            part = strtok(nullptr, "/");
            continue;
        }

        FAT32Directory dir(_dev, *_clusters, currentDirCluster);
        bool found = false;
        
        while (dir.next(outEntry)) {
            if (matches(outEntry, part)) {
                found = true;
                break;
            }
        }

        if (!found) return false;

        part = strtok(nullptr, "/");
        if (part != nullptr) {
            if (!(outEntry.attributes & 0x10)) return false; // Not a dir
            currentDirCluster = outEntry.firstCluster;
            if (currentDirCluster == 0) currentDirCluster = _bpb.rootCluster();
        }
    }
    return true;
}

IFile* FAT32FileSystem::openFile(const char* path) {
    FAT32DirectoryEntry entry;
    if (resolvePath(path, entry)) {
        if (!(entry.attributes & FAT_ATTR_DIRECTORY)) {
            return new FAT32File(_dev, *_clusters, entry);
        }
    }
    return nullptr;
}

// Factory for the Iterator
IDirIterator* FAT32FileSystem::openDir(const char* path) {
    uint32_t cluster;
    
    if (strcmp(path, "/") == 0) {
        cluster = _bpb.rootCluster();
    } else {
        FAT32DirectoryEntry entry;
        if (!resolvePath(path, entry)) return nullptr;
        if (!(entry.attributes & 0x10)) return nullptr; // Not a directory
        cluster = entry.firstCluster;
        if (cluster == 0) cluster = _bpb.rootCluster();
    }
    
    // Return a new concrete iterator cast to the interface
    return new FAT32Iterator(_dev, *_clusters, cluster);
}
    
// Check if a path exists
bool FAT32FileSystem::exists(const char* path) {
    if (strcmp(path, "/") == 0) return true; // Root always exists
    FAT32DirectoryEntry entry;
    return resolvePath(path, entry);
}