#pragma once

#include "fat32_file.h"
#include "../../block/iblock_device.h"
#include "../vfs/ifile_system.h"
#include "fat32_iterator.h"

class FAT32FileSystem : public IFileSystem {
public:
    FAT32FileSystem(IBlockDevice& dev) : _dev(dev), _clusters(nullptr) {}

    bool mount() override;

    IFile* openFile(const char* path) override {
        FAT32DirectoryEntry entry;
        if (resolvePath(path, entry) && !(entry.attributes & 0x10)) {
            return new FAT32File(_dev, *_clusters, entry);
        }
        return nullptr;
    }

    IDirIterator* openDir(const char* path) override {
        // Root directory special case
        if (strcmp(path, "/") == 0) 
            return new FAT32DirectoryIterator(_dev, *_clusters, _bpb.rootCluster());

        FAT32DirectoryEntry entry;
        if (resolvePath(path, entry) && (entry.attributes & 0x10)) {
            return new FAT32DirectoryIterator(_dev, *_clusters, entry.firstCluster);
        }
        return nullptr;
    }

private:
    // Helper that walks the directory tree for a given absolute path
    bool resolvePath(const char* path, FAT32DirectoryEntry& out);

    IBlockDevice& _dev;
    FAT32BPB _bpb;
    FAT32ClusterTable* _clusters;
};