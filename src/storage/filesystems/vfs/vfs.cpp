#include "storage/filesystems/vfs/vfs.h"

#include <string.h>
#include <stdio.h>

bool VFS::mount(const char* prefix, IFileSystem* fs) {
    if (_mountCount >= 4) return false;
    
    _mounts[_mountCount].prefix = prefix;
    _mounts[_mountCount].fs = fs;
    strcpy(_mounts[_mountCount].cwd, "/"); // Default to root
    
    _mountCount++;
    return true;
}

// Internal helper to split "SD:/DIR/FILE" into MountPoint(SD) and "/DIR/FILE"
MountPoint* VFS::findMount(const char* path, const char** internalPath) {
    const char* colon = strchr(path, ':');
    
    if (colon) {
        // Explicit drive: "SD0:/FILE"
        size_t prefixLen = colon - path;
        for (int i = 0; i < _mountCount; i++) {
            if (strlen(_mounts[i].prefix) == prefixLen && 
                strncmp(_mounts[i].prefix, path, prefixLen) == 0) {
                *internalPath = colon + 1;
                return &_mounts[i];
            }
        }
        return nullptr; // Named drive not found
    } else {
        // Relative to current drive: "FILE"
        *internalPath = path;
        return &_mounts[_currentDrive];
    }
}

bool VFS::unmount(const char* prefix) {
    for (int i = 0; i < _mountCount; i++) {
        if (strcmp(_mounts[i].prefix, prefix) == 0) {
            // 1. Delete the filesystem object
            delete _mounts[i].fs; 
            
            // Note: FAT32FileSystem should probably delete the Volume 
            // it was given in its destructor to keep things clean.

            _mountCount--;
            return true;
        }
    }
    return false;
}

IFile* VFS::open(const char* path) {
    const char* internalPath = nullptr;
    MountPoint* mnt = findMount(path, &internalPath);
    if (!mnt) return nullptr;

    char fullPath[VFS_MAX_PATH_LENGTH];
    resolveFullPath(mnt, internalPath, fullPath);

    return mnt->fs->openFile(fullPath);
}

IDirIterator* VFS::openDir(const char* path) {
    const char* internalPath = nullptr;
    MountPoint* mnt = findMount(path, &internalPath);
    if (!mnt) return nullptr;

    char fullPath[VFS_MAX_PATH_LENGTH];
    resolveFullPath(mnt, internalPath, fullPath);

    return mnt->fs->openDir(fullPath);
}

bool VFS::cd(const char* path) {
    // 1. Check if this is just a drive switch (e.g., "SD0:")
    const char* colon = strchr(path, ':');
    if (colon && *(colon + 1) == '\0') {
        char prefix[16];
        size_t len = colon - path;
        strncpy(prefix, path, len);
        prefix[len] = '\0';
        
        for (int i = 0; i < _mountCount; i++) {
            if (strcmp(_mounts[i].prefix, prefix) == 0) {
                _currentDrive = i;
                return true;
            }
        }
        return false;
    }

    // 2. Normal directory change
    const char* internalPath = nullptr;
    MountPoint* mnt = findMount(path, &internalPath);
    if (!mnt) return false;

    char targetPath[VFS_MAX_PATH_LENGTH];
    resolveFullPath(mnt, internalPath, targetPath);

    // 3. Verify the directory exists before moving there
    if (mnt->fs->exists(targetPath)) {
        strncpy(mnt->cwd, targetPath, VFS_MAX_PATH_LENGTH);
        return true;
    }

    return false;
}

const char* VFS::getCwd() {
    static char fullCwd[VFS_MAX_PATH_LENGTH + 16];
    MountPoint& mnt = _mounts[_currentDrive];
    
    snprintf(fullCwd, sizeof(fullCwd), "%s:%s", mnt.prefix, mnt.cwd);
    return fullCwd;
}

void VFS::resolveFullPath(MountPoint* mnt, const char* input, char* output) {
    // 1. If input starts with '/', it's absolute relative to the drive
    if (input[0] == '/') {
        strncpy(output, input, VFS_MAX_PATH_LENGTH);
    } else {
        // 2. It's relative. Combine CWD and input
        strncpy(output, mnt->cwd, VFS_MAX_PATH_LENGTH);
        
        // Add a separator if the CWD isn't just "/"
        if (output[strlen(output)-1] != '/') {
            strncat(output, "/", VFS_MAX_PATH_LENGTH - strlen(output));
        }
        strncat(output, input, VFS_MAX_PATH_LENGTH - strlen(output));
    }

    // 3. Handle ".." (Parent directory) logic
    cleanPath(output);
}

void VFS::cleanPath(char* path) {
    // Simple logic: if path ends in "/..", remove the last two segments
    char* dotdot = strstr(path, "/..");
    if (dotdot) {
        *dotdot = '\0'; // Cut the string at the dots
        char* lastSlash = strrchr(path, '/');
        if (lastSlash) {
            if (lastSlash == path) *(lastSlash + 1) = '\0'; // Keep root slash
            else *lastSlash = '\0';
        }
    }
}