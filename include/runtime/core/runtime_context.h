#pragma once

#include "value.h"
#include "utils/managed_arena.h"
#include <vector>

struct ForLoopFrame {
    Symbol variable;
    float endValue;
    float stepValue;
    int bodyStartLine;
};

struct RuntimeContext {
    // Variables
    std::vector<Value> variables;
    size_t variableCount;
    std::vector<ForLoopFrame> forLoopStack;

    // Stack/Heap emulation
    Arena transientArena;
    ManagedArena persistentArena;

    RuntimeContext(size_t variableCount, size_t persistent, size_t transient) : 
        variableCount(variableCount),
        persistentArena(persistent), 
        transientArena(transient)
        { 
            variables.reserve(variableCount);
        }
    
    // Jump control
    bool jumpRequested = false;
    int jumpTarget = -1;
    
    // Program control
    int currentLine = 0;
    Value returnValue;
    bool halted = false;

    void reset(){
        halted = false;
        returnValue = Value::Float(0.0f);
        transientArena.reset();
        persistentArena.reset();
        forLoopStack.clear();
        for (size_t i = 0; i < variableCount; i++) variables[i] = Value::Float(0.0f);
    }

    Value promoteValue(Value val) {
        // Only strings in the Transient Arena need to be moved to Persistent
        if (val.type == ValueType::TransientString && val.rawPtr) {
            
            char* newStorage = persistentArena.dupString(
                val.rawPtr, 
                val.length, 
                variables // Pass the roots so they stay valid if GC hits
            );

            if (newStorage) {
                return Value::PersistentStr(newStorage, val.length);
            }
        }
        return val;
    }

    Value* get(Symbol id){
        return &variables[id];
    }

    void assign(Symbol id, Value val) {
        // 1. One branch to ensure size (usually false, very fast)
        if (id >= variables.size()) {
            variables.resize(id + 1, Value::Float(0.0f));
        }
        
        // 2. One branch for string check
        if (val.type == ValueType::TransientString) {
            val = promoteValue(val);
        }
        
        // 3. Direct write to contiguous memory
        variables[id] = val;
    }
};