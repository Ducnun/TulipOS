#pragma once

#include <string>
#include <stdint.h>
#include <sstream>

enum ValueType {
    Integer,
    String,
    Boolean,
    Float,
    None
};

struct VariableValue {
    // Variable Value has a 
    ValueType type = ValueType::None;
    int16_t intValue;
    std::string stringValue;
    bool boolValue;
    float floatValue;

    VariableValue() : type(ValueType::None), intValue(0), boolValue(false), floatValue(0.2f) {}

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

    inline std::string toString() const {
        char buffer[32];
        switch(type){
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