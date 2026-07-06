#pragma once

#include <vector>
#include "arena.h"
#include "runtime/core/value.h"

class ManagedArena {
public:
    ManagedArena(size_t size) 
        : arenaA(size), arenaB(size), activeArena(&arenaA) {}

    void* alloc(size_t size, std::vector<Value>& roots) {
        void* ptr = activeArena->alloc(size);
        
        if (!ptr) {
            // Out of memory! Trigger GC
            collect(roots);
            
            // Try again in the new arena
            ptr = activeArena->alloc(size);
            
            if (!ptr) {
                // Still no room? We are truly out of memory
                return nullptr;
            }
        }
        return ptr;
    }

    void reset() {
        arenaA.reset();
        arenaB.reset();
        activeArena = &arenaA;
    }

private:
    Arena arenaA;
    Arena arenaB;
    Arena* activeArena;

    void collect(std::vector<Value>& roots) {
        // 1. Pick the "other" arena as the destination
        Arena* targetArena = (activeArena == &arenaA) ? &arenaB : &arenaA;
        targetArena->reset();

        // 2. Iterate through all variables (the "Roots")
        for (Value& val : roots) {
            // Only move strings that are in the persistent storage
            if (val.type == ValueType::PersistentString && val.rawPtr) {
                char* newLoc = (char*)targetArena->alloc(val.length + 1);
                if (newLoc) {
                    memcpy(newLoc, val.rawPtr, val.length + 1);
                    val.rawPtr = newLoc; // The "Magic": Update the pointer to new home
                }
            }
        }

        // 3. Swap which arena we use for future allocations
        activeArena = targetArena;
        
        // Note: The "old" arena isn't reset here (to be safe), 
        // but it will be reset next time it becomes the target.
    }
};