#pragma once

#include <stddef.h>
#include <vector>
#include "utils/arena.h"
#include "utils/managed_arena.h"
#include "runtime/core/value.h"

class RuntimeResources {
public:
    RuntimeResources(void* srcBuf, size_t srcSize,
                     void* codeBuf, size_t codeSize,
                     void* tranBuf, size_t tranSize,
                     void* symBuf,  size_t symSize,
                     void* persABuf, void* persBBuf, size_t persSize){
        srcArena.init(srcBuf, srcSize);
        codeArena.init(codeBuf, codeSize);
        tranArena.init(tranBuf, tranSize);
        symArena.init(symBuf, symSize);
        persArena.init(persABuf, persBBuf, persSize);

        // 1. Carve out the Variable Array from the Symbol Arena
        // This ensures variables never touch the system heap.
        _maxVariables = symSize / sizeof(Value);
        _variables = (Value*)symArena.alloc(_maxVariables * sizeof(Value));
        
        resetVariables();
    }

    // Arenas are public for the Parser/Interpreter to use for scratchpad/AST
    Arena srcArena;
    Arena codeArena;
    Arena tranArena;
    Arena symArena;
    ManagedArena persArena;

    // Promotion logic: The Interpreter calls this when assigning a string
    inline Value promoteValue(Value val) {
        if (val.type == ValueType::TransientString && val.rawPtr) {
            // Pass our internal _variables array as the "roots" 
            // so the ManagedArena can update pointers if it needs to GC/Compact
            char* newStorage = persArena.dupString(val.rawPtr,  val.length,  _variables, _maxVariables);
            if (newStorage) return Value::PersistentStr(newStorage, val.length);
        }
        return val;
    }
    
    void resetArenas() {
        srcArena.reset();
        tranArena.reset();
        symArena.reset();
        persArena.reset();
    }

    void resetVariables() {
        for (size_t i = 0; i < _maxVariables; i++) {
            _variables[i] = Value::Float(0.0f);
        }
    }

    // Accessors for the RuntimeContext
    Value* getVariables() { return _variables; }
    size_t getVariableCount() { return _maxVariables; }

private:
    Value* _variables;
    size_t _maxVariables;
};