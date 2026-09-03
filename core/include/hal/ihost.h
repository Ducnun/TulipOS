#pragma once

#include <string>
#include <cstdint>
#include <vector>

#include "iblock_device.h"
#include "imemory.h"

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

    // --- Hardware Discovery ---
    virtual std::vector<StorageDescriptor> getAvailableStorage() = 0;

    // --- Platform dependent Services ---
    virtual IMemoryManager* getMemory() = 0;

    /**
     * @brief Services any background tasks during busy waiting
     */
    virtual void service() = 0;

    /**
     * @brief Prints to IO
     * @param txt 
     */
    virtual void print(const std::string& txt) = 0;

    /**
     * @brief Reads line from IO until '\n'
     * @return std::string 
     */
    virtual std::string readLine() = 0;

    /**
     * @brief Clears display buffer
     */
    virtual void clearScreen() = 0;

    /**
     * @brief Resets the state of the IO
     */
    virtual void reset() = 0;

    /**
     * @brief Set the Cursor object
     * @param x 
     * @param y 
     */
    virtual void setCursor(int x, int y) = 0;

    /**
     * @brief Checks if the pause/break key is pressed (ESP32 only). Used for interrupting programs
     * @return true Key press is read and buffered
     * @return false Not pressed
     */
    virtual bool checkBreakRequested() {return false;}

    virtual void delay(uint32_t ms) = 0;
    virtual uint32_t millis() = 0;
};