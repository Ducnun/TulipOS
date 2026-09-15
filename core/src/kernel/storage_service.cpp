#include "kernel/storage_service.h"
#include <cstring>

// --- Directory Operations ---
bool StorageService::changeDirectory(const char* path)  {
    return _vfs.cd(path);
}

size_t StorageService::listDirectory(const char* path, util::span<DirectoryEntry> buffer){
    const char* targetPath = path ? path : _vfs.getCwd();
    IDirIterator* dir = _vfs.openDir(targetPath);
    if (!dir) return 0; // Failed to open directory

    size_t count = 0;
    DirectoryEntry entry;
    while (dir->next(entry) && count < buffer.size()){
        buffer[count++] = entry; // Copy entry into the span
    }

    delete dir;
    return count; // Number of entries written
}

// --- File Operations ---
bool StorageService::fileExists(const char* path)  {
    IFile* file = _vfs.open(path);
    if (!file) return false;
    delete file;
    return true;
}

size_t StorageService::getFileSize(const char* path)  {
    IFile* file = _vfs.open(path);
    if (!file) return 0;
    size_t size = file->size();
    delete file;
    return size;
}

bool StorageService::readFile(const char* path, uint8_t* buffer, size_t size, size_t offset)  {
    IFile* file = _vfs.open(path);
    if (!file) return false;
    file->seek(offset);
    size_t bytesRead = file->read(buffer, size);
    delete file;
    return (bytesRead == size);
}

// --- Program Loading ---
bool StorageService::loadProgram(const char* path, uint8_t** buffer, size_t* size)  {
    size_t fileSize = getFileSize(path);
    if (fileSize == 0) return false;

    *buffer = (uint8_t*)_memory.alloc(fileSize, MemoryType::Program);
    if (!*buffer) return false;

    *size = fileSize;
    return readFile(path, *buffer, fileSize, 0);
}

void StorageService::freeProgram(uint8_t* buffer)  {
    free(buffer);
}

// --- Mount/Unmount ---
bool StorageService::mount(util::StringView prefix, IBlockDevice& device){
    FilesystemType type = detectFilesystem(device);
    return mount(prefix, device, type);
}

bool StorageService::mount(util::StringView prefix, IBlockDevice& device, FilesystemType type){
    IFileSystem* fs = createFileSystem(device, type);
    if (!fs) return false;

    char prefixBuf[16];
    size_t len = std::min(prefix.length, sizeof(prefixBuf) - 1);
    strncpy(prefixBuf, prefix.data, len);
    prefixBuf[len] = '\0';

    bool success = _vfs.mount(prefixBuf, fs);
    if (!success) {
        delete fs; // Clean up on failure
    }
    return success;
}

bool StorageService::unmount(util::StringView prefix){
    char prefixBuf[16];
    size_t len = std::min(prefix.length, sizeof(prefixBuf) - 1);
    strncpy(prefixBuf, prefix.data, len);
    prefixBuf[len] = '\0';
    return _vfs.unmount(prefixBuf);
}

// Helpers for auto mounting
FilesystemType StorageService::detectFilesystem(IBlockDevice& device){
    uint8_t sector[512];
    if (!device.readBlock(0, sector)) {
        return FilesystemType::UnsupportedFS;
    }

    // Check for FAT32 (0x55AA at offset 510)
    if (sector[510] == 0x55 && sector[511] == 0xAA) {
        return FilesystemType::Fat32;
    }

    return FilesystemType::Raw;
}

IFileSystem* StorageService::createFileSystem(IBlockDevice& device, FilesystemType type){
    switch (type) {
        case FilesystemType::Fat32:
            return new FAT32FileSystem(device);
        case FilesystemType::Raw:
        default:
            return nullptr;
    }
}