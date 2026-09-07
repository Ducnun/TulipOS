#pragma once

#include "value.h"
#include "runtime_resources.h"
#include <vector>
#include <algorithm>

struct ForLoopFrame {
    Symbol variable;
    float endValue;
    float stepValue;
    int bodyStartLine;
};

/**
 * @brief Represents the volatile execution state of the interpreter.
 * It does not own memory; it merely tracks the Program Counter, 
 * control flow stacks, and refers to the Memory Manager (Resources).
 */
struct RuntimeContext {
    // Reference to the Memory Manager
    RuntimeResources& res;

    // Direct access to variables (stored in the Resource's Symbol Arena)
    Value* const variables;
    const size_t variableCount;
    
    // Control Flow Stacks
    // these could eventually be moved to a FixedStack or an Arena.
    std::vector<ForLoopFrame> forLoopStack;

    // Execution State (The "Registers")
    int currentLine = 0;
    bool halted = false;
    bool jumpRequested = false;
    int jumpTarget = -1;
    Value returnValue;

    RuntimeContext(RuntimeResources& resources) :
        res(resources),
        variables(resources.getVariables()),
        variableCount(resources.getVariableCount())
    { 
        // Initial state
        returnValue = Value::Float(0.0f);
    }
    
    /**
     * @brief Resets the VM state for a new 'RUN'
     */
    void reset() {
        halted = false;
        currentLine = 0;
        jumpRequested = false;
        jumpTarget = -1;
        returnValue = Value::Float(0.0f);
        
        forLoopStack.clear();

        // Delegate memory clearing to the Resource Manager
        res.resetArenas();
        res.resetVariables();
    }

    /**
     * @brief Gets a pointer to a variable by its Symbol ID
     */
    inline Value* get(Symbol id) {
        // In a debug build, you might want to assert(id < variableCount)
        return &variables[id];
    }

    /**
     * @brief Assigns a value to a variable, handling string promotion automatically
     */
    inline void assign(Symbol id, Value val) {
        // The Resource manager handles moving strings from Transient to Persistent
        variables[id] = res.promoteValue(val);
    }
};