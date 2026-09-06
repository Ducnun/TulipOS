#pragma once

#include <vector>
#include "arena.h"
#include "../runtime/core/value.h"

class ManagedArena {
public:
    ManagedArena() : activeArena(&arenaA) {}

    void init(void* bufA, void* bufB, size_t size) {
        arenaA.init(bufA, size);
        arenaB.init(bufB, size);
        activeArena = &arenaA;
    }

    void* alloc(size_t size, std::vector<Value>& roots) {
        size_t total = size + sizeof(StringHeader);
        void* block = activeArena->alloc(total);
        
        if (!block) {
            collect(roots); 
            block = activeArena->alloc(total);
            if (!block) return nullptr;
        }

        StringHeader* header = (StringHeader*)block;
        header->forwardingAddr = nullptr;

        return (char*)block + sizeof(StringHeader);
    }

    char* dupString(const char* src, size_t len, std::vector<Value>& roots) {
        // TRAP: If src is already in the activeArena, a GC trigger in alloc() 
        // will move src, making our pointer invalid before we can memcpy it!
        
        bool srcIsManaged = activeArena->contains((void*)src);
        
        // If it's managed, we need to know its offset so we can find it after GC
        uint32_t srcOffset = 0;
        if (srcIsManaged) {
            // This is a rare case in our BASIC (usually src is Transient), 
            // but we handle it for safety.
            srcOffset = (uint32_t)((uint8_t*)src - (uint8_t*)activeArena->getPointer(0));
        }

        char* dest = (char*)alloc(len + 1, roots); // <--- GC might happen here
        
        if (!dest) return nullptr;

        // If GC happened, and src was managed, we must find its NEW location
        const char* finalSrc = src;
        if (srcIsManaged) {
            // We find the new pointer by looking at the "old" arena which is now the inactive one
            Arena* oldArena = (activeArena == &arenaA) ? &arenaB : &arenaA;
            void* oldHeaderLoc = (uint8_t*)oldArena->getPointer(srcOffset) - sizeof(StringHeader);
            finalSrc = ((StringHeader*)oldHeaderLoc)->forwardingAddr;
        }

        memcpy(dest, finalSrc, len);
        dest[len] = '\0';
        return dest;
    }

    void collect(std::vector<Value>& roots) {
        Arena* targetArena = (activeArena == &arenaA) ? &arenaB : &arenaA;
        targetArena->reset();

        for (Value& val : roots) {
            if (val.type != ValueType::PersistentString || !val.rawPtr) continue;
            
            // Only move strings that belong to the arena we are about to clear
            if (!activeArena->contains((void*)val.rawPtr)) continue; 

            StringHeader* oldHeader = (StringHeader*)(val.rawPtr - sizeof(StringHeader));

            if (oldHeader->forwardingAddr) {
                // Shared string: already moved
                val.rawPtr = oldHeader->forwardingAddr;
            } else {
                // New string: move it
                size_t dataSize = val.length + 1;
                void* newBlock = targetArena->alloc(dataSize + sizeof(StringHeader));
                
                if (newBlock) {
                    char* newData = (char*)newBlock + sizeof(StringHeader);
                    ((StringHeader*)newBlock)->forwardingAddr = nullptr;
                    
                    memcpy(newData, val.rawPtr, dataSize);
                    oldHeader->forwardingAddr = newData;
                    val.rawPtr = newData;
                }
            }
        }
        activeArena = targetArena;
    }

    void reset() {
        arenaA.reset();
        arenaB.reset();
        activeArena = &arenaA;
    }

private:
    struct StringHeader {
        char* forwardingAddr; 
    };

    Arena arenaA;
    Arena arenaB;
    Arena* activeArena;
};