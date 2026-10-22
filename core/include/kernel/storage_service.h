#pragma once

#include "kernel/api/istorage_service.h"
#include "kernel/api/imemory_service.h"
#include "storage/filesystems/fat32/fat32_filesystem.h"

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
    void freeProgram(uint8_t* buffer, size_t size);

    // --- Mount/Unmount ---
    bool mount(util::StringView prefix, IBlockDevice& device);
    bool mount(util::StringView prefix, IBlockDevice& device, FilesystemType type);
    bool unmount(util::StringView prefix);

private:
    VFS& _vfs;
    IMemoryService& _memory;

    FilesystemType detectFilesystem(IBlockDevice& device);
    IFileSystem* createFileSystem(IBlockDevice& device, FilesystemType type);
};