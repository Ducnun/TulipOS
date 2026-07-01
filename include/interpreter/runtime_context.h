#pragma once

#include <unordered_map>

#include "value.h"

struct ForLoopFrame {
    std::string variable;
    float endValue;
    float stepValue;
    int bodyStartLine;
};

struct RuntimeContext {
    // Variables
    std::unordered_map<std::string, Value> variables;
    std::vector<ForLoopFrame> forLoopStack;
    
    // Jump control
    bool jumpRequested = false;
    int jumpTarget = -1;
    
    // Program control
    int currentLine = 0;
    Value returnValue;
    bool halted = false;

    void set(const std::string& name, Value val){
        variables[name] = std::move(val);
    }

    Value get(const std::string& name) const{
        auto it = variables.find(name);
        if (it == variables.end()) {
            return Value(0.0f); // BASIC default value
        }
        return it->second;
    }

    void clear(){
        variables.clear();
    }

    void reset(){
        clear();
        halted = false;
        returnValue = Value(0.0f);
    }
};