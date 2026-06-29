#pragma once
#include <string>
#include <iostream>
#include <sstream>

enum class ValueType { Number, String, Boolean };

struct Value {
    ValueType type;
    double numberValue;
    std::string stringValue;
    bool boolValue;

    Value() : type(ValueType::Number), numberValue(0.0), boolValue(false) {}
    Value(double val) : type(ValueType::Number), numberValue(val), boolValue(val != 0) {}
    Value(std::string val) : type(ValueType::String), numberValue(0.0), boolValue(!val.empty()), stringValue(std::move(val)) {}
    Value(bool val) : type(ValueType::Boolean), numberValue(val ? 1.0 : 0.0), boolValue(val) {}

    bool isTruthy() const {
        switch (type) {
            case ValueType::Boolean: return boolValue;
            case ValueType::Number:  return numberValue != 0.0;
            case ValueType::String:  return !stringValue.empty();
            default: return false;
        }
    }

    std::string asString() const {
        if (type == ValueType::String) return stringValue;
        if (type == ValueType::Boolean) return boolValue ? "TRUE" : "FALSE";
        std::stringstream ss;
        ss << numberValue;
        return ss.str();
    }
    
    double asNumber() const {
        if (type == ValueType::Number) return numberValue;
        if (type == ValueType::Boolean) return boolValue ? 1.0 : 0.0;
        try { return std::stod(stringValue); } catch (...) { return 0.0; }
    }
};