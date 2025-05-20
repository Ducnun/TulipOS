#pragma once

#include <string>
#include <stdint.h>
#include <sstream>

enum ValueType {
    NoValType,
    Integer,
    String,
    Boolean,
    Float
};

struct VariableValue {
    // Variable Value has a 
    ValueType type = ValueType::NoValType;
    int16_t intValue;
    std::string stringValue;
    bool boolValue;
    float floatValue;

    VariableValue() : type(ValueType::NoValType), intValue(0), boolValue(false), floatValue(0.2f) {}

    static VariableValue fromInt(int16_t v){
        VariableValue val;
        val.type = ValueType::Integer;
        val.intValue = v;
        return val;
    }

    static VariableValue fromString(std::string v){
        VariableValue val;
        val.type = ValueType::String;
        val.stringValue = v;
        return val;
    }

    static VariableValue fromBool(bool v){
        VariableValue val;
        val.type = ValueType::Boolean;
        val.boolValue = v;
        return val;
    }

    static VariableValue fromFloat(float v){
        VariableValue val;
        val.type = ValueType::Float;
        val.floatValue = v;
        return val;
    }

    bool toBool() const {
        switch (type){
            case ValueType::Integer: return intValue != 0; break;
            case ValueType::Float: return floatValue != 0.0f; break;
            case ValueType::String: return !stringValue.empty(); break;
            case ValueType::Boolean: return boolValue; break;
        }
        return false;
    }

    inline std::string toString() const {
        char buffer[32];
        switch (type){
            case ValueType::Integer:
                return std::to_string(intValue);
            case ValueType::Boolean:
                return boolValue ? "TRUE" : "FALSE";
            case ValueType::Float:
                snprintf(buffer, sizeof(buffer), "%.6g", floatValue);
                return buffer;
            case ValueType::String:
                return stringValue;
            default:
                return "?UNDEFINED";
        }
    }

};