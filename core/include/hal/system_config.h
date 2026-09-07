#pragma once

#include <stddef.h>

struct SystemConfig{
    size_t sourceSize;
    size_t codeSize;
    size_t transientSize;
    size_t persistentSize;
    size_t symbolSize;
};