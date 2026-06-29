#pragma once

#include <unordered_map>

#include "value.h"

class RuntimeContext {
private:
    std::unordered_map<std::string, Value> variables;

public:
    void set(const std::string& name, Value val) {
        variables[name] = std::move(val);
    }

    Value get(const std::string& name) const {
        auto it = variables.find(name);
        if (it == variables.end()) {
            return Value(0.0); // BASIC default value
        }
        return it->second;
    }

    void clear() {
        variables.clear();
    }
};