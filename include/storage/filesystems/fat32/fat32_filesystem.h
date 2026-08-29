#pragma once

#include "fat32_file.h"
#include "../../block/iblock_device.h"
#include "../vfs/ifile_system.h"
#include "fat32_iterator.h"

class FAT32FileSystem : public IFileSystem {
public:
    FAT32FileSystem(IBlockDevice& dev) : _dev(dev), _clusters(nullptr) {}
    
    // Implement all interface methods
    bool mount() override;
    IFile* openFile(const char* path) override;
    IDirIterator* openDir(const char* path) override;
    bool exists(const char* path) override;

private:
    bool resolvePath(const char* path, FAT32DirectoryEntry& out);

    IBlockDevice& _dev;
    FAT32BPB _bpb;
    FAT32ClusterTable* _clusters;
};