#pragma once

#include <string>
#include <stdint.h>
#include <sstream>
#include <memory>

enum VariableType {
    NoValType,
    Integer,
    String,
    Float
};

class Variable {
public:
    virtual ~Variable() {}
    virtual std::string toString() const = 0;
    virtual VariableType getType() const = 0;
    virtual int16_t toBool() const = 0;
    virtual std::unique_ptr<Variable> clone() const = 0;
};

class IntVariable : public Variable {
public:
    int16_t value;
    IntVariable(int16_t v): value (v){}
    std::string toString() const override { return std::to_string(value); }
    VariableType getType() const override { return VariableType::Integer; }
    int16_t toBool() const override { return value != 0 ? -1 : 0; }
    std::unique_ptr<Variable> clone() const override { return std::unique_ptr<IntVariable>(new IntVariable(value)); }
};

class StringVariable : public Variable {
public:
    std::string value;
    StringVariable(std::string v): value (v){}
    std::string toString() const override { return value; }
    VariableType getType() const override { return VariableType::String; }
    int16_t toBool() const override { return !value.empty() ? -1 : 0; }
    std::unique_ptr<Variable> clone() const override { return std::unique_ptr<StringVariable>(new StringVariable(value)); }
};

class FloatVariable : public Variable {
public:
    float value;
    FloatVariable(float v): value (v){}
    std::string toString() const override { return std::to_string(value); }
    VariableType getType() const override { return VariableType::Float; }
    int16_t toBool() const override { return value != 0 ? -1 : 0; }
    std::unique_ptr<Variable> clone() const override { return std::unique_ptr<FloatVariable>(new FloatVariable(value)); }
};

