#pragma once
#include <vector>
#include <string>
#include "storage/filesystems/vfs/vfs.h"
#include "hal/ihost.h"

class StorageService {
public:
    StorageService(VFS& vfs) : _vfs(vfs) {}

    // Initialize/Handoff hardware
    void mountHardware(const std::vector<StorageDescriptor>& hardware);

    // High-level API for the Shell/Engine
    IFile* open(const std::string& path);
    bool exists(const std::string& path);
    bool cd(const std::string& path);
    std::string getCwd() const;

    // Directory listing (Raw data, no formatting!)
    std::vector<DirectoryEntry> listDirectory(const std::string& path = ".");

private:
    VFS& _vfs;
};