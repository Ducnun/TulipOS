#pragma once

#include <cstdint>
#include <vector>

#include "iterminal.h"
#include "iblock_device.h"
#include "ikeyboard.h"
#include "imemory.h"

struct StorageDescriptor {
    const util::StringView defaultPrefix; // e.g. "SD"
    IBlockDevice* device;      // The actual hardware (SDCardSPI or PCDiskImage)
};

class IHost {
public: 
    /**
     * @brief Destroy the IHost object
     */
    virtual ~IHost() = default;

    // --- Hardware Discovery ---
    virtual std::vector<StorageDescriptor> getAvailableStorage() = 0;
    
    // --- Platform dependent Services ---
    virtual IKeyboard& keyboard() = 0;
    virtual ITerminal& terminal() = 0;
    virtual IMemoryManager& memory() = 0;

    virtual void begin() = 0;

    /**
     * @brief Services any background tasks during busy waiting
     */
    virtual void service() = 0;

    /**
     * @brief Checks if the pause/break key is pressed (ESP32 only). Used for interrupting programs
     * @return true Key press is read and buffered
     * @return false Not pressed
     */
    virtual bool checkBreakRequested() {return false;}

    virtual void delay(uint32_t ms) = 0;
    virtual uint32_t millis() = 0;
};