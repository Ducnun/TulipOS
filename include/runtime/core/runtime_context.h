#pragma once

#include <unordered_map>

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
    std::unordered_map<Symbol, Value> variables;
    std::vector<ForLoopFrame> forLoopStack;

    // Stack/Heap emulation
    Arena transientArena;
    Arena persistentArena;

    RuntimeContext(size_t persistent, size_t transient) 
        : persistentArena(persistent), transientArena(transient) {}
    
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
        variables.clear();
        forLoopStack.clear();
    }

    Value promoteValue(Value val){
        if (val.type == ValueType::TransientString && val.rawPtr){
            const char* dest = persistentArena.dupString(val.rawPtr);
            return Value::PersistentStr(dest, val.length);
        }
        return val;
    }
};