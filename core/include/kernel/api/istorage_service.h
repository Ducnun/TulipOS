#pragma once

#include <cstdint>
#include <cstddef>
#include "storage/filesystems/vfs/vfs.h"
#include "hal/iblock_device.h"
#include "utils/span.h"
#include "utils/stringview.h"

enum FilesystemType {
    UnsupportedFS,
    Fat32,
    Raw,
};

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
    virtual void freeProgram(uint8_t* buffer, size_t size) = 0;

    // --- Mounting and unmounting ---
    virtual bool mount(util::StringView prefix, IBlockDevice& device) = 0;
    virtual bool mount(util::StringView prefix, IBlockDevice& device, FilesystemType type) = 0;
    virtual bool unmount(util::StringView prefix) = 0;
};