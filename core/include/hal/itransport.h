#pragma once

class ITransport{
public:
    ~ITransport() = default;

    virtual bool begin() = 0;
    
};