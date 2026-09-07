#pragma once

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

    /**
     * @brief Allocates space, triggers GC if full.
     */
    void* alloc(size_t size, Value* roots, size_t rootCount) {
        size_t total = size + sizeof(StringHeader);
        void* block = activeArena->alloc(total);
        
        if (!block) {
            collect(roots, rootCount); 
            block = activeArena->alloc(total);
            if (!block) return nullptr; // Hard OOM
        }

        StringHeader* header = (StringHeader*)block;
        header->forwardingAddr = nullptr;

        return (char*)block + sizeof(StringHeader);
    }

    /**
     * @brief Copies a string into persistent storage, updating roots if GC hits.
     */
    char* dupString(const char* src, size_t len, Value* roots, size_t rootCount) {
        bool srcIsManaged = activeArena->contains((void*)src);
        uint32_t srcOffset = 0;
        
        if (srcIsManaged) {
            srcOffset = (uint32_t)((uint8_t*)src - (uint8_t*)activeArena->getPointer(0));
        }

        // Pass raw pointer/count to alloc
        char* dest = (char*)alloc(len + 1, roots, rootCount);
        
        if (!dest) return nullptr;

        const char* finalSrc = src;
        if (srcIsManaged) {
            // After potential GC, locate new address of src via forwarding addr
            Arena* oldArena = (activeArena == &arenaA) ? &arenaB : &arenaA;
            void* oldDataLoc = oldArena->getPointer(srcOffset);
            void* oldHeaderLoc = (uint8_t*)oldDataLoc - sizeof(StringHeader);
            finalSrc = ((StringHeader*)oldHeaderLoc)->forwardingAddr;
        }

        memcpy(dest, finalSrc, len);
        dest[len] = '\0';
        return dest;
    }

    /**
     * @brief Semi-space Copying Garbage Collector
     */
    void collect(Value* roots, size_t rootCount) {
        Arena* targetArena = (activeArena == &arenaA) ? &arenaB : &arenaA;
        targetArena->reset();

        for (size_t i = 0; i < rootCount; ++i) {
            Value& val = roots[i];
            
            // Only process persistent strings that live in the arena we are clearing
            if (val.type != ValueType::PersistentString || !val.rawPtr) continue;
            if (!activeArena->contains((void*)val.rawPtr)) continue; 

            StringHeader* oldHeader = (StringHeader*)(val.rawPtr - sizeof(StringHeader));

            if (oldHeader->forwardingAddr) {
                // String already moved by a previous root
                val.rawPtr = oldHeader->forwardingAddr;
            } else {
                // Move string to the new arena
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