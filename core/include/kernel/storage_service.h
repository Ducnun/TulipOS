#pragma once

#include "kernel/api/istorage_service.h"
#include "kernel/api/imemory_service.h"

class StorageService : public IStorageService{
public:
    StorageService(VFS& vfs, IMemoryService& memory) : _vfs(vfs), _memory(memory) {}

    // --- Directory Operations ---
    bool changeDirectory(const char* path);
    const char* getCurrentDirectory() { return _vfs.getCwd(); };
    size_t listDirectory(const char* path, util::span<DirectoryEntry> buffer);

    // --- File Operations ---
    bool fileExists(const char* path);
    size_t getFileSize(const char* path);
    bool readFile(const char* path, uint8_t* buffer, size_t size, size_t offset);

    // --- Program Loading ---
    bool loadProgram(const char* path, uint8_t** buffer, size_t* size);
    void freeProgram(uint8_t* buffer);

    // --- Mount/Unmount ---
    bool mount(const char* prefix, IFileSystem* fs);
    bool unmount(const char* prefix);

private:
    VFS& _vfs;
    IMemoryService& _memory;
};