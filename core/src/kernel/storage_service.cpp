#include "kernel/storage_service.h"
#include "storage/mbr/mbr.h"
#include <cstring>
#include <memory>
#include <utility>

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
    if (!path || (!buffer && size > 0)) return false;

    std::unique_ptr<IFile> file(_vfs.open(path));
    if (!file) return false;
    if (!file->seek(offset)) return false;
    size_t bytesRead = file->read(buffer, size);
    return (bytesRead == size);
}

// --- Program Loading ---
bool StorageService::loadProgram(const char* path, uint8_t** buffer, size_t* size)  {
    if (!buffer || !size) return false;
    *buffer = nullptr;
    *size = 0;
    if (!path) return false;

    size_t fileSize = getFileSize(path);
    if (fileSize == 0) return false;

    auto* programBuffer = static_cast<uint8_t*>(
        _memory.alloc(fileSize, MemoryType::Program));
    if (!programBuffer) return false;

    if (!readFile(path, programBuffer, fileSize, 0)) {
        _memory.free(programBuffer, fileSize, MemoryType::Program);
        return false;
    }

    *buffer = programBuffer;
    *size = fileSize;
    return true;
}

void StorageService::freeProgram(uint8_t* buffer, size_t size)  {
    _memory.free(buffer, size, MemoryType::Program);
}

// --- Mount/Unmount ---
bool StorageService::mount(util::StringView prefix, IBlockDevice& device){
    if (!prefix.data || prefix.length == 0 || prefix.length >= 16) return false;

    FilesystemType type = detectFilesystem(device);
    return mount(prefix, device, type);
}

bool StorageService::mount(util::StringView prefix, IBlockDevice& device, FilesystemType type){
    if (!prefix.data || prefix.length == 0 || prefix.length >= 16) return false;

    IFileSystem* fs = createFileSystem(device, type);
    if (!fs) return false;

    char prefixBuf[16];
    memcpy(prefixBuf, prefix.data, prefix.length);
    prefixBuf[prefix.length] = '\0';

    if (!fs->mount()) {
        delete fs;
        return false;
    }

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

    FAT32BPB bpb;
    if (bpb.parse(sector)) {
        return FilesystemType::Fat32;
    }

    MBR mbr;
    if (!mbr.read(device)) {
        return FilesystemType::UnsupportedFS;
    }

    for (size_t i = 0; i < mbr.partitionCount(); ++i) {
        const Partition& partition = mbr.getPartition(i);
        const uint8_t type = partition.type();
        if (type == 0x0B || type == 0x0C ||
            type == 0x1B || type == 0x1C) {
            return FilesystemType::Fat32;
        }
    }

    return FilesystemType::UnsupportedFS;
}

IFileSystem* StorageService::createFileSystem(IBlockDevice& device, FilesystemType type){
    switch (type) {
        case FilesystemType::Fat32: {
            uint8_t sector[512];
            if (!device.readBlock(0, sector)) return nullptr;

            FAT32BPB bpb;
            if (bpb.parse(sector)) {
                return new FAT32FileSystem(device);
            }

            MBR mbr;
            if (!mbr.read(device)) return nullptr;

            for (size_t i = 0; i < mbr.partitionCount(); ++i) {
                const Partition& partition = mbr.getPartition(i);
                const uint8_t type = partition.type();
                if (type != 0x0B && type != 0x0C &&
                    type != 0x1B && type != 0x1C) {
                    continue;
                }

                std::unique_ptr<IBlockDevice> volume(
                    new Volume(device, partition));
                uint8_t bootSector[512];
                if (!volume->readBlock(0, bootSector)) {
                    continue;
                }

                FAT32BPB partitionBpb;
                if (!partitionBpb.parse(bootSector)) {
                    continue;
                }

                return new FAT32FileSystem(std::move(volume));
            }
            return nullptr;
        }
        case FilesystemType::Raw:
        default:
            return nullptr;
    }
}