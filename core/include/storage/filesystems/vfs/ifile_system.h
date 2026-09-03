#pragma once

#include "ifile.h"
#include "idir_iterator.h"

class IFileSystem {
public:
    virtual ~IFileSystem() {}
    virtual bool mount() = 0;
    
    // Returns a handle to read/write data
    virtual IFile* openFile(const char* path) = 0;

    // Returns a handle to list files in a folder
    virtual IDirIterator* openDir(const char* path) = 0;

    // Check if a path is valid (file or folder)
    virtual bool exists(const char* path) = 0;
};