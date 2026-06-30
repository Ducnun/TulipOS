#pragma once

#include <unordered_map>

#include "value.h"

struct RuntimeContext {
    std::unordered_map<std::string, Value> variables;
    bool halted = false;
    Value returnValue;

    void set(const std::string& name, Value val){
        variables[name] = std::move(val);
    }

    Value get(const std::string& name) const{
        auto it = variables.find(name);
        if (it == variables.end()) {
            return Value(0.0); // BASIC default value
        }
        return it->second;
    }

    void clear(){
        variables.clear();
    }

    void reset(){
        clear();
        halted = false;
        returnValue = Value(0.0);
    }
};