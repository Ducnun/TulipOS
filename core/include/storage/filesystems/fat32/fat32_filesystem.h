#pragma once

#include "fat32_file.h"
#include "hal/iblock_device.h"
#include "../vfs/ifile_system.h"
#include "fat32_iterator.h"
#include "storage/block/volume.h"

#include <memory>

#pragma pack(push, 4)
class FAT32FileSystem : public IFileSystem {
public:
    explicit FAT32FileSystem(IBlockDevice& dev)
        : _dev(dev), _clusters(nullptr) {}
    explicit FAT32FileSystem(std::unique_ptr<IBlockDevice> dev)
        : _ownedDevice(std::move(dev)), _dev(*_ownedDevice), _clusters(nullptr) {}
    ~FAT32FileSystem() override { delete _clusters; }
    
    // Implement all interface methods
    bool mount() override;
    IFile* openFile(const char* path) override;
    IDirIterator* openDir(const char* path) override;
    bool exists(const char* path) override;

private:
    bool resolvePath(const char* path, FAT32DirectoryEntry& out);

    std::unique_ptr<IBlockDevice> _ownedDevice;
    IBlockDevice& _dev;
    FAT32BPB _bpb;
    FAT32ClusterTable* _clusters;
};

#pragma pack(pop)