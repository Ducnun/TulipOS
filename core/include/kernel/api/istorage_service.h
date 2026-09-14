#pragma once

#include <cstdint>
#include <cstddef>
#include "storage/filesystems/vfs/vfs.h"
#include "utils/span.h"

class IStorageService {
public:
    virtual ~IStorageService() = default;

    // --- Directory Operations ---
    virtual bool changeDirectory(const char* path) = 0;
    virtual const char* getCurrentDirectory() = 0;
    virtual size_t listDirectory(const char* path, util::span<DirectoryEntry> buffer) = 0;

    // --- File Operations ---
    virtual bool fileExists(const char* path) = 0;
    virtual size_t getFileSize(const char* path) = 0;
    virtual bool readFile(const char* path, uint8_t* buffer, size_t size, size_t offset) = 0;

    // --- Program Loading ---
    virtual bool loadProgram(const char* path, uint8_t** buffer, size_t* size) = 0;
    virtual void freeProgram(uint8_t* buffer) = 0;

    // --- Mount/Unmount ---
    virtual bool mount(const char* prefix, IFileSystem* fs) = 0;
    virtual bool unmount(const char* prefix) = 0;
};