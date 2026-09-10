#pragma once

#include "value.h"
#include "runtime_resources.h"
#include "identifier_id.h"
#include <vector>
#include <algorithm>

struct ForLoopFrame {
    IdentifierId identifier;
    float endValue;
    float stepValue;
    int bodyStartLine;
};

/**
 * @brief Represents the volatile execution state of the interpreter.
 * It does not own memory; it merely tracks the Program Counter, 
 * control flow stacks, and refers to the Memory Manager (Resources).
 */// In RuntimeContext.h
struct RuntimeContext {
    RuntimeResources& res;
    std::vector<ForLoopFrame> forLoopStack;

    // Execution State
    int currentLine = 0;
    bool halted = false;
    bool jumpRequested = false;
    int jumpTarget = -1;
    Value returnValue;

    RuntimeContext(RuntimeResources& resources) : res(resources) {
        returnValue = Value::Float(0.0f);
    }

    void reset() {
        halted = false;
        currentLine = 0;
        jumpRequested = false;
        jumpTarget = -1;
        returnValue = Value::Float(0.0f);
        forLoopStack.clear();
    }

    // --- Variable Access (by name or ID) ---
    inline Value getVariableValue(StringView name) {
        return res.getVariableValue(res.getOrCreateVariable(name));
    }

    inline void setVariableValue(StringView name, Value val) {
        VariableId varId = res.getOrCreateVariable(name);
        res.setVariableValue(varId, res.promoteValue(val));
    }

    // If you already have a VariableId (e.g., from parsing)
    inline Value getVariableValue(VariableId varId) {
        return res.getVariableValue(varId);
    }

    inline void setVariableValue(VariableId varId, Value val) {
        res.setVariableValue(varId, res.promoteValue(val));
    }

    // For loops: access by IdentifierId (the loop frame holds the name ID)
    inline void setForLoopVariable(IdentifierId nameId, Value val) {
        VariableId varId = res.getOrCreateVariable(nameId);
        setVariableValue(varId, val);
    }
};