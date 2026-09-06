#pragma once

#include "storage/filesystems/vfs/ifile.h"

class IStorage{
public:
    virtual IFile* open(const std::string& path) = 0;
    virtual bool exists(const std::string& path) = 0;
};