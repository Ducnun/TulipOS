#pragma once

#include <string>
#include <stdint.h>
#include <sstream>
#include <memory>

/**
 * @brief Enum of variable types supported
 */
enum VariableType {
    NoValType,
    Integer,
    String,
    Float
};

struct Variable{
    VariableType type;
    
    static Variable fromInt(int val) { return Variable(static_cast<int16_t>(val)); }
    static Variable fromFloat(double val) { return Variable(static_cast<float>(val)); }

    union {
        int16_t intValue;
        float floatValue;
    };
    std::string stringValue;

    Variable() : type(VariableType::NoValType), intValue(0) {}
    Variable(int16_t val) : type(VariableType::Integer), intValue(val) {}
    Variable(float val) : type(VariableType::Float), floatValue(val) {}
    Variable(const std::string& val) : type(VariableType::String), stringValue(val) {}

    Variable(const Variable& other);

    // Overloaded operators
    Variable& operator=(const Variable& other);
    Variable operator+(const Variable& other) const;
    Variable operator-(const Variable& other) const;
    Variable operator*(const Variable& other) const;
    Variable operator/(const Variable& other) const;
    Variable operator^(const Variable& other) const;
    bool operator==(const Variable& other) const;
    bool operator!=(const Variable& other) const;
    bool operator<(const Variable& other) const;
    bool operator<=(const Variable& other) const;
    bool operator>(const Variable& other) const;
    bool operator>=(const Variable& other) const;

    ~Variable();
    
    std::string toString() const;
    int16_t toBool() const;
    int16_t toInt() const;
    float toFloat() const;
};
