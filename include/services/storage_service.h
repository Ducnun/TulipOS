#pragma once

#include "storage/mbr/mbr.h"
#include "storage/block/volume.h"
#include "storage/filesystems/fs_list.h"
#include "storage/filesystems/vfs/vfs.h"
#include "storage/filesystems/vfs/ifile.h"
#include "storage/filesystems/vfs/idir_iterator.h"
#include "host/ihost.h"

class StorageService{
public:
    StorageService(VFS& vfs, IHost& host) : _vfs(vfs), _host(host) {}
    void initializeStorage();

    void ls(const char* path = nullptr);

private:
    VFS& _vfs;
    IHost& _host;  
};