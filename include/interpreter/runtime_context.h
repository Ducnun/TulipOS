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
    
    // Jump control
    bool jumpRequested = false;
    int jumpTarget = -1;
    
    // Program control
    int currentLine = 0;
    Value returnValue;
    bool halted = false;

    void clear(){
        variables.clear();
        forLoopStack.clear();
    }

    void reset(){
        clear();
        halted = false;
        returnValue = Value::Float(0.0f);
    }
};