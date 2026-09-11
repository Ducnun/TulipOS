#pragma once

#include "utils/span.h"

class ITransport {
public:
    virtual ~ITransport() = default;
    virtual bool begin() = 0;
    virtual bool send(const util::span<const uint8_t>& data) = 0;
    virtual bool receive(uint8_t& byte) { return false; }
};