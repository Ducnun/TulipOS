#pragma once

#include <string>
#include <memory> // For shared_ptr

#include "symbol_table.h"

enum class ValueType : uint8_t { Number, Boolean, PermanentString, TransientString, PersistentString };

struct Value {
    ValueType type;         // 1 byte explicitly
    uint16_t length;        // 2 bytes
    uint8_t flags;          // 1 byte reserved for future

    union{                  // Ensures that the 4 byte union is not split over registers
        float number;
        bool boolean;
        Symbol stringId;
        const char* rawPtr;
    };

    // Factories
    static Value Float(float val){
        Value v;
        v.type = ValueType::Number;
        v.number = val;
        v.length = 0;
        return v;
    }

    static Value Bool(bool val){
        Value v;
        v.type = ValueType::Boolean;
        v.boolean = val;
        v.length = 0;
        return v;
    }

    static Value PermanentStr(Symbol id, uint16_t length){
        Value v;
        v.type = ValueType::PermanentString;
        v.stringId = id;
        v.length = length;
        return v;
    }

    static Value TransientStr(const char* strPtr, uint16_t length){
        Value v;
        v.type = ValueType::TransientString;
        v.rawPtr = strPtr;
        v.length = length;
        return v;
    }

    static Value PersistentStr(const char* strPtr, uint16_t length){
        Value v;
        v.type = ValueType::PersistentString;
        v.rawPtr = strPtr;
        v.length = length;
        return v;
    }

    bool isTruthy() const {
        switch (type) {
            case ValueType::Boolean:            return boolean;
            case ValueType::Number:             return number != 0.0f;
            case ValueType::PermanentString:    return stringId != INVALID_SYMBOL;
            case ValueType::TransientString:    return rawPtr != 0;
            case ValueType::PersistentString:   return rawPtr != nullptr && rawPtr[0] != '\0';
            default: return false;
        }
    }
};