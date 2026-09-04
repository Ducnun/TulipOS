#pragma once

#include <string>
#include <cstdint>
#include <vector>

#include "iblock_device.h"
#include "imemory.h"
#include "ikeyboard.h"
#include "iterminal.h"

struct StorageDescriptor {
    const char* defaultPrefix; // e.g. "SD"
    IBlockDevice* device;      // The actual hardware (SDCardSPI or PCDiskImage)
};

class IHost {
public: 
    /**
     * @brief Destroy the IHost object
     */
    virtual ~IHost() = default;

    // --- Expose HAL services/drivers ---
    virtual std::vector<StorageDescriptor> getAvailableStorage() = 0;
    virtual IMemoryManager* getMemory() = 0;
    virtual ITerminal* getTerminal() = 0;
    virtual IKeyboard* getKeyboard() = 0;

    /**
     * @brief Services any background tasks during busy waiting
     */
    virtual void service() = 0;

    /**
     * @brief Executes platform dependent delay
     */
    virtual void delay(uint32_t ms) = 0;

    /**
     * @brief Executes platform dependent "millis" (ms since boot)
     */
    virtual uint32_t millis() = 0;
};