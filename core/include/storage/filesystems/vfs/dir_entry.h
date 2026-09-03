#pragma once
#include <stdint.h>
#include <stddef.h>

// Generic entry for directory listing
struct DirectoryEntry {
    char name[32]; // Standardized name (e.g. "FILE.TXT")
    uint32_t size;
    bool isDirectory;
};