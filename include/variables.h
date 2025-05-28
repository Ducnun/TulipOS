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

class Variable {
public:
    /**
     * @brief Destroy the Variable object
     */
    virtual ~Variable() {}

    /**
     * @brief Formats the variable to a string
     * @return std::string 
     */
    virtual std::string toString() const = 0;

    /**
     * @brief Get the Type of the variable
     * @return VariableType representation of the variables type
     */
    virtual VariableType getType() const = 0;

    /**
     * @brief Translates the type to a boolean representation for logic
     * @return int16_t The byte representation of a C64 BASIC style boolean 
     */
    virtual int16_t toBool() const = 0;

    /**
     * @brief Clone function to make copying of variables easier
     * @return std::unique_ptr<Variable> pointer to the new cloned variable
     */
    virtual std::unique_ptr<Variable> clone() const = 0;
};

class IntVariable : public Variable {
public:
    int16_t value;  ///< Datafield that holds the actual variable data
    /**
     * @brief Construct a new Int Variable object
     * @param v Initial value
     */
    IntVariable(int16_t v): value (v){}

    /** \copydoc Variable::toString */
    std::string toString() const override { return std::to_string(value); }
    /** \copydoc Variable::getType */
    VariableType getType() const override { return VariableType::Integer; }
    /** \copydoc Variable::toBool */
    int16_t toBool() const override { return value != 0 ? -1 : 0; }
    /** \copydoc Variable::clone */
    std::unique_ptr<Variable> clone() const override { return std::unique_ptr<IntVariable>(new IntVariable(value)); }
};

class StringVariable : public Variable {
public:
    std::string value;  ///< Datafield that holds the actual variable data
    /**
     * @brief Construct a new String Variable object
     * @param v Initial value
     */
    StringVariable(std::string v): value (v){}
    /** \copydoc Variable::toString */
    std::string toString() const override { return value; }
    /** \copydoc Variable::getType */
    VariableType getType() const override { return VariableType::String; }
    /** \copydoc Variable::toBool */
    int16_t toBool() const override { return !value.empty() ? -1 : 0; }
    /** \copydoc Variable::clone */
    std::unique_ptr<Variable> clone() const override { return std::unique_ptr<StringVariable>(new StringVariable(value)); }
};

class FloatVariable : public Variable {
public:
    float value;    ///< Datafield that holds the actual variable data
    /**
     * @brief Construct a new Float Variable object
     * @param v Initial value
     */
    FloatVariable(float v): value (v){}
    /** \copydoc Variable::toString */
    std::string toString() const override { return std::to_string(value); }
    /** \copydoc Variable::getType */
    VariableType getType() const override { return VariableType::Float; }
    /** \copydoc Variable::toBool */
    int16_t toBool() const override { return value != 0 ? -1 : 0; }
    /** \copydoc Variable::clone */
    std::unique_ptr<Variable> clone() const override { return std::unique_ptr<FloatVariable>(new FloatVariable(value)); }
};

