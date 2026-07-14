#pragma once

#include "value.h"
#include "utils/managed_arena.h"
#include "host/ihost.h"
#include <vector>

struct ForLoopFrame {
    Symbol variable;
    float endValue;
    float stepValue;
    int bodyStartLine;
};

struct RuntimeContext {
    IHost& host;

    // Variables
    std::vector<Value> variables;
    Value* varPtr = nullptr;
    size_t variableCount;
    std::vector<ForLoopFrame> forLoopStack;

    // Stack/Heap emulation
    Arena transientArena;
    ManagedArena persistentArena;

    RuntimeContext(IHost& host, size_t variableCount, size_t persistent, size_t transient) : 
        host(host),
        variableCount(variableCount),
        persistentArena(persistent), 
        transientArena(transient)
        { 
            variables.resize(variableCount);
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
        
        std::fill(variables.begin(), variables.end(), Value::Float(0.0f));
        varPtr = variables.data();
    }

    inline Value promoteValue(Value val) {
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

    inline Value* get(Symbol id){
        return &varPtr[id];
    }

    inline void assign(Symbol id, Value val) {
        if (val.type == ValueType::TransientString) {
            val = promoteValue(val);
        }
        varPtr[id] = val;
    }
};