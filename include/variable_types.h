#pragma once

#include <string>

enum class ValueType {
    Integer,
    String,
    Boolean,
    None
};

struct VariableValue {
    // Variable Value has a 
    ValueType type = ValueType::None;
    int intValue;
    std::string stringValue;
    bool boolValue;

    VariableValue() : type(ValueType::None), intValue(0), boolValue(false) {}

    static VariableValue fromInt(int v){
        VariableValue val;
        val.type = ValueType::Integer;
    }
};