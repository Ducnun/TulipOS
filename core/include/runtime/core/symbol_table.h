#pragma once

#include <cstdint>
#include <cstring>
#include "../../utils/arena.h"
#include "../../utils/stringview.h"

using Symbol = uint16_t;
const Symbol INVALID_SYMBOL = 0xFFFF;

class SymbolTable {
public:
    /**
     * @brief Construct the table within a fixed arena.
     * @param arena The arena to use for metadata and string storage.
     * @param maxSymbols The maximum number of unique variables allowed.
     */
    SymbolTable(Arena& arena, uint16_t maxSymbols = 128) : _arena(arena) {
        _maxSymbols = maxSymbols;
        
        // 1. Allocate the names array (The "Vector")
        _names = (const char**)_arena.alloc(sizeof(const char*) * _maxSymbols);
        
        // 2. Allocate the hash bucket array (The "Map")
        // We make the bucket size ~2x the max symbols to keep hash collisions low
        _bucketCount = maxSymbols * 2;
        _buckets = (HashEntry*)_arena.alloc(sizeof(HashEntry) * _bucketCount);
        
        clear();
    }

    Symbol getOrCreate(util::StringView sv) {
        uint32_t h = sv.hash();
        uint32_t index = h % _bucketCount;

        // Linear probing for the hash table
        for (uint32_t i = 0; i < _bucketCount; i++) {
            uint32_t slot = (index + i) % _bucketCount;
            
            if (_buckets[slot].id == INVALID_SYMBOL) {
                // Not found, create new
                return createEntry(sv, slot);
            }

            // Check if this slot matches the name
            const char* existingName = _names[_buckets[slot].id];
            if (sv.equals(existingName)) {
                return _buckets[slot].id;
            }
        }

        return INVALID_SYMBOL; // Table Full
    }

    const char* getString(Symbol sym) const {
        if (sym >= _symbolCount) return "";
        return _names[sym];
    }

    void clear() {
        _symbolCount = 0;
        // Initialize buckets to invalid
        for (uint32_t i = 0; i < _bucketCount; i++) {
            _buckets[i].id = INVALID_SYMBOL;
        }
        // Note: We don't reset the arena here because the 
        // metadata arrays live at the start of it.
    }

private:
    struct HashEntry {
        Symbol id;
    };

    Arena& _arena;
    const char** _names;     // Array of pointers to strings in the arena
    HashEntry* _buckets;     // Hash table for O(1) lookup
    uint16_t _maxSymbols;
    uint16_t _symbolCount;
    uint32_t _bucketCount;

    Symbol createEntry(util::StringView sv, uint32_t slot) {
        if (_symbolCount >= _maxSymbols) return INVALID_SYMBOL;

        // Allocate the string characters in the remaining arena space
        char* storage = (char*)_arena.alloc(sv.length + 1);
        if (!storage) return INVALID_SYMBOL;

        memcpy(storage, sv.data, sv.length);
        storage[sv.length] = '\0';

        Symbol newId = _symbolCount++;
        _names[newId] = storage;
        _buckets[slot].id = newId;

        return newId;
    }
};