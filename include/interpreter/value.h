#pragma once
#include <string>
#include <memory> // For shared_ptr

enum class ValueType { Number, String, Boolean };

struct Value {
    ValueType type;
    float numberValue;
    bool boolValue;
    std::shared_ptr<std::string> stringValue;

    Value() : type(ValueType::Number), numberValue(0.0f), boolValue(false), stringValue(nullptr) {}
    
    Value(float val) : type(ValueType::Number), numberValue(val), boolValue(val != 0.0f), stringValue(nullptr) {}
    Value(bool val) : type(ValueType::Boolean), numberValue(val ? 1.0f : 0.0f), boolValue(val), stringValue(nullptr) {}
    Value(std::string val) : type(ValueType::String), numberValue(0.0f), boolValue(!val.empty()) { stringValue = std::make_shared<std::string>(std::move(val));}

    bool isNumber()  const { return type == ValueType::Number; }
    bool isString()  const { return type == ValueType::String; }
    bool isBoolean() const { return type == ValueType::Boolean; }

    bool isTruthy() const {
        switch (type) {
            case ValueType::Boolean: return boolValue;
            case ValueType::Number:  return numberValue != 0.0f;
            case ValueType::String:  return stringValue && !stringValue->empty();
            default: return false;
        }
    }

    std::string asString() const {
        if (type == ValueType::String && stringValue) return *stringValue;
        if (type == ValueType::Boolean) return boolValue ? "TRUE" : "FALSE";
        
        // Optimized float to string
        char buf[16];
        snprintf(buf, sizeof(buf), "%g", numberValue);
        return std::string(buf);
    }
    
    float asNumber() const {
        if (type == ValueType::Number) return numberValue;
        if (type == ValueType::Boolean) return boolValue ? 1.0f : 0.0f;
        if (type == ValueType::String && stringValue) {
            try { 
                return std::stof(*stringValue); 
            } catch (...) { return 0.0f; }
        }
        return 0.0f;
    }
};