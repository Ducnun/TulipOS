#pragma once

#include "ifile_system.h"
#include <vector>

#define VFS_MAX_PATH_LENGTH 128

struct MountPoint{
    const char* prefix;
    IFileSystem* fs;
    char cwd[VFS_MAX_PATH_LENGTH];
};

class VFS {
public:
    bool mount(const char* prefix, IFileSystem* fs);
    bool unmount(const char* prefix);

    IFile* open(const char* path);
    IDirIterator* openDir(const char* path);

    bool cd(const char* path);
    const char* getCwd(void);

private:
    MountPoint* findMount(const char* path, const char** internalPath);
    void resolveFullPath(MountPoint* mnt, const char* input, char* output);
    void cleanPath(char* path);

    MountPoint _mounts[4];
    int _mountCount;
    int _currentDrive;
};
