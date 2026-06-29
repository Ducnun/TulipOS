#pragma once
#include <string>
#include <iostream>
#include <sstream>

enum class ValueType { Number, String };

struct Value {
    ValueType type;
    double numberValue;
    std::string stringValue;

    // Constructors
    Value() : type(ValueType::Number), numberValue(0.0) {}
    Value(double val) : type(ValueType::Number), numberValue(val) {}
    Value(std::string val) : type(ValueType::String), numberValue(0.0), stringValue(std::move(val)) {}
    Value(const char* val) : type(ValueType::String), numberValue(0.0), stringValue(val) {}

    bool isNumber() const { return type == ValueType::Number; }
    bool isString() const { return type == ValueType::String; }

    double asNumber() const {
        if (type == ValueType::Number) return numberValue;
        try {
            return std::stod(stringValue);
        } catch (...) {
            return 0.0;
        }
    }

    std::string asString() const {
        if (type == ValueType::String) return stringValue;
        // Use stringstream for clean float formatting on Arduino/ESP32
        std::stringstream ss;
        ss << numberValue;
        return ss.str();
    }

    // Equality helpers
    bool operator==(const Value& other) const {
        if (type != other.type) return false;
        if (type == ValueType::Number) return numberValue == other.numberValue;
        return stringValue == other.stringValue;
    }
};