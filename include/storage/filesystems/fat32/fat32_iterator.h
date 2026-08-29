#pragma once

#include "../vfs/idir_iterator.h"
#include "storage/block/iblock_device.h"
#include "fat32_directory.h"

class FAT32Iterator : public IDirIterator {
public:
    FAT32Iterator(IBlockDevice& dev, FAT32ClusterTable& clusters, uint32_t firstCluster)
        : _dir(dev, clusters, firstCluster) {}

    bool next(DirectoryEntry& entry) override {
        FAT32DirectoryEntry fatEntry;
        if (!_dir.next(fatEntry)) return false;

        // Convert FAT-specific "NAME    EXT" to VFS "NAME.EXT"
        int p = 0;
        for (int i = 0; i < 8 && fatEntry.name[i] != ' '; i++) entry.name[p++] = fatEntry.name[i];
        if (fatEntry.extension[0] != ' ') {
            entry.name[p++] = '.';
            for (int i = 0; i < 3 && fatEntry.extension[i] != ' '; i++) entry.name[p++] = fatEntry.extension[i];
        }
        entry.name[p] = '\0';

        entry.size = fatEntry.fileSize;
        entry.isDirectory = (fatEntry.attributes & FAT_ATTR_DIRECTORY);
        return true;
    }

private:
    FAT32Directory _dir; // The low-level logic we built
};