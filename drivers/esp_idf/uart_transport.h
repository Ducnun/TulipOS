#pragma once

#include "hal/itransport.h"
#include "driver/uart.h"

class UARTTransport : public ITransport{
public:
    UARTTransport(uint32_t tx, uint32_t rx) : _tx(tx), _rx(rx) {}

    bool begin();
    bool send(const util::span<const uint8_t>& data);
    bool receive(uint8_t& byte) { return false; };

private:
    uint32_t _tx;
    uint32_t _rx;
};