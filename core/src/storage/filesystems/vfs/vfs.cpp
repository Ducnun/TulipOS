#include "storage/filesystems/vfs/vfs.h"

#include <string.h>
#include <stdio.h>

bool VFS::mount(const char* prefix, IFileSystem* fs) {
    if (!prefix || !fs || _mountCount >= 4) return false;

    const size_t prefixLength = strlen(prefix);
    if (prefixLength == 0 || prefixLength >= VFS_MAX_PREFIX_LENGTH) return false;

    MountPoint& mount = _mounts[_mountCount];
    memcpy(mount.prefix, prefix, prefixLength + 1);
    mount.fs = fs;
    strcpy(mount.cwd, "/");

    if (_currentDrive < 0 || _currentDrive >= _mountCount) {
        _currentDrive = _mountCount;
    }
    _mountCount++;
    return true;
}

// Internal helper to split "SD:/DIR/FILE" into MountPoint(SD) and "/DIR/FILE"
MountPoint* VFS::findMount(const char* path, const char** internalPath) {
    if (!path || !internalPath) return nullptr;

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
        if (_currentDrive < 0 || _currentDrive >= _mountCount) return nullptr;
        *internalPath = path;
        return &_mounts[_currentDrive];
    }
}

bool VFS::unmount(const char* prefix) {
    if (!prefix) return false;

    for (int i = 0; i < _mountCount; i++) {
        if (strcmp(_mounts[i].prefix, prefix) == 0) {
            delete _mounts[i].fs;
            for (int j = i; j < _mountCount - 1; ++j) {
                _mounts[j] = _mounts[j + 1];
            }
            _mountCount--;
            _mounts[_mountCount] = {};

            if (_mountCount == 0) {
                _currentDrive = -1;
            } else if (_currentDrive == i) {
                _currentDrive = i < _mountCount ? i : _mountCount - 1;
            } else if (_currentDrive > i) {
                _currentDrive--;
            }
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
    if (!path) return false;

    // 1. Check if this is just a drive switch (e.g., "SD0:")
    const char* colon = strchr(path, ':');
    if (colon && *(colon + 1) == '\0') {
        char prefix[16];
        size_t len = colon - path;
        if (len >= sizeof(prefix)) return false;
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
    if (_currentDrive < 0 || _currentDrive >= _mountCount) return "";

    static char fullCwd[VFS_MAX_PATH_LENGTH + 16];
    MountPoint& mnt = _mounts[_currentDrive];
    
    snprintf(fullCwd, sizeof(fullCwd), "%s:%s", mnt.prefix, mnt.cwd);
    return fullCwd;
}

void VFS::resolveFullPath(MountPoint* mnt, const char* input, char* output) {
    // Start with the base (CWD or Root)
    if (input[0] == '/') {
        strncpy(output, input, VFS_MAX_PATH_LENGTH);
    } else if (strcmp(input, ".") == 0 || strlen(input) == 0) {
        strncpy(output, mnt->cwd, VFS_MAX_PATH_LENGTH);
    } else {
        strncpy(output, mnt->cwd, VFS_MAX_PATH_LENGTH);
        if (output[strlen(output)-1] != '/') strncat(output, "/", VFS_MAX_PATH_LENGTH);
        strncat(output, input, VFS_MAX_PATH_LENGTH);
    }

    // This removes trailing dots, double slashes, and handles ".."
    cleanPath(output);
}

void VFS::cleanPath(char* path) {
    size_t len = strlen(path);
    if (len == 0) return;

    // Remove trailing slash (unless it's the root "/")
    if (len > 1 && path[len - 1] == '/') {
        path[len - 1] = '\0';
        len--;
    }

    // Handle "/." (e.g., "/DATA/." -> "/DATA")
    if (len >= 2 && strcmp(&path[len - 2], "/.") == 0) {
        path[len - 2] = '\0';
        if (strlen(path) == 0) strcpy(path, "/"); // Ensure we don't end up empty
    }
    
    // If the path is strictly ".", it's the root
    if (strcmp(path, ".") == 0) strcpy(path, "/");
}