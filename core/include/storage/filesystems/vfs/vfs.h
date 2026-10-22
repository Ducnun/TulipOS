#pragma once

#include "ifile_system.h"
#include <vector>

#define VFS_MAX_PATH_LENGTH 128
#define VFS_MAX_PREFIX_LENGTH 16

struct MountPoint {
    char prefix[VFS_MAX_PREFIX_LENGTH];
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

    const char* getActiveDrivePrefix() {
        return (_currentDrive >= 0 && _currentDrive < _mountCount)
                   ? _mounts[_currentDrive].prefix
                   : nullptr;
    };

    int getMountCount(void) { return _mountCount; };
    void setActiveDrive(int driveNr) {
        if (driveNr >= 0 && driveNr < _mountCount) {
            _currentDrive = driveNr;
        }
    };

private:
    MountPoint* findMount(const char* path, const char** internalPath);
    void resolveFullPath(MountPoint* mnt, const char* input, char* output);
    void cleanPath(char* path);

    MountPoint _mounts[4] = {};
    int _mountCount = 0;
    int _currentDrive = -1;
};
