#pragma once

#include "storage/mbr/mbr.h"
#include "storage/block/volume.h"
#include "storage/filesystems/fs_list.h"
#include "storage/filesystems/vfs/vfs.h"
#include "storage/filesystems/vfs/ifile.h"
#include "storage/filesystems/vfs/idir_iterator.h"
#include "hal/ihost.h"

class StorageService{
public:
    StorageService(VFS& vfs, IHost& host) : _vfs(vfs), _host(host) {}
    void initializeStorage();
    
    // Core commands
    IFile* open(const char* path);
    void ls(const char* path = nullptr);
    bool cd(const char* path = nullptr);
    
    // utils
    const char* getCwd(void) {return _vfs.getCwd(); };
    void formatSize(uint32_t bytes, char* out, size_t outSize);

private:
    VFS& _vfs;
    IHost& _host;  
};