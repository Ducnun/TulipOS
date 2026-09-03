#pragma once

#include "stddef.h"
#include "stdint.h"

class IFile {
public:
    virtual ~IFile() {}
    virtual size_t read(uint8_t* buffer, size_t len) = 0;
    virtual bool seek(uint32_t pos) = 0;
    virtual uint32_t size() const = 0;
    virtual bool eof() const = 0;
    virtual void close() = 0;
};