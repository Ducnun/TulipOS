#pragma once

#include <string>
#include <memory> // For shared_ptr

#include "engine/symboltable.h"

enum class ValueType { Number, String, Boolean };

struct Value {
    ValueType type;
    union{
        float number;
        bool boolean;
        Symbol stringId;
    };

    // Factories
    static Value Float(float val){
        Value v;
        v.type = ValueType::Number;
        v.number = val;
        return v;
    }

    static Value Bool(bool val){
        Value v;
        v.type = ValueType::Boolean;
        v.boolean = val;
        return v;
    }

    static Value Str(Symbol id){
        Value v;
        v.type = ValueType::String;
        v.stringId = id;
        return v;
    }

    bool isTruthy() const {
        switch (type) {
            case ValueType::Boolean: return boolean;
            case ValueType::Number:  return number != 0.0f;
            case ValueType::String:  return stringId != INVALID_SYMBOL;
            default: return false;
        }
    }
};