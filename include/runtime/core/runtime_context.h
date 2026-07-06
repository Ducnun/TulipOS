#pragma once

#include "value.h"
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
    Arena persistentArena;

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

    Value promoteValue(Value val){
        // If the string is transient, the actual string values are stored in the scratchpad
        // They need to move to the persistent arena so its not wiped after this line
        if (val.type == ValueType::TransientString && val.rawPtr){
            char* dest = (char*)persistentArena.alloc(val.length + 1);

            // Copy over to persistent arena and return Value with adjusted ptr location for string data
            if (dest){
                memcpy(dest, val.rawPtr, val.length + 1);
                return Value::PersistentStr(dest, val.length);
            }
        }
        // If simple type or already persistent/permanent string, nothing has to be done
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