#pragma once

#include <stddef.h>
#include <stdint.h>

#include "dir_entry.h"

class IDirIterator {
public:
    virtual ~IDirIterator() {}
    virtual bool next(DirectoryEntry& entry) = 0;
};