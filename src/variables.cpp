#include "variables.h"

Variable::~Variable(){
}

Variable::Variable(const Variable& other) : type(other.type){
    switch (other.type) {
        case VariableType::Integer:
            intValue = other.intValue;
            break;
        case VariableType::Float:
            floatValue = other.floatValue;
            break;
        case VariableType::String:
            new (&stringValue) std::string(other.stringValue);
            break;
        default:
            break;
    }
}

Variable& Variable::operator=(const Variable& other){
    if (this != &other) {
        this->~Variable(); // clean up first
        type = other.type;
        switch (other.type) {
            case VariableType::Integer:
                intValue = other.intValue;
                break;
            case VariableType::Float:
                floatValue = other.floatValue;
                break;
            case VariableType::String:
                new (&stringValue) std::string(other.stringValue);
                break;
            default:
                break;
        }
    }
    return *this;
}

Variable Variable::operator+(const Variable& other) const{
    if (type == VariableType::String || other.type == VariableType::String){
        return Variable(this->toString() + other.toString());
    }
    else if (type == VariableType::Float || other.type == VariableType::Float){
        return Variable(this->toFloat() + other.toFloat());
    }
    else{
        return Variable::fromInt(this->toInt() + other.toInt());
    }
}

Variable Variable::operator-(const Variable& other) const{
    if (type == VariableType::String || other.type == VariableType::String){
        throw std::runtime_error("Cannot apply '-' to strings");
    }
    else if (type == VariableType::Float || other.type == VariableType::Float){
        return Variable(this->toFloat() - other.toFloat());
    }
    else{
        return Variable::fromInt(this->toInt() - other.toInt());
    }
}

Variable Variable::operator*(const Variable& other) const{
    if (type == VariableType::String || other.type == VariableType::String){
        throw std::runtime_error("Cannot apply '*' to strings");
    }
    else if (type == VariableType::Float || other.type == VariableType::Float){
        return Variable(this->toFloat() * other.toFloat());
    }
    else{
        return Variable::fromInt(this->toInt() * other.toInt());
    }
}

Variable Variable::operator/(const Variable& other) const{
    if (type == VariableType::String || other.type == VariableType::String){
        throw std::runtime_error("Cannot apply '/' to strings");
    }
    else if (type == VariableType::Float || other.type == VariableType::Float){
        return Variable(this->toFloat() / other.toFloat());
    }
    else{
        return Variable::fromInt(this->toInt() / other.toInt());
    }
}

Variable Variable::operator^(const Variable& other) const{
    if (type == VariableType::String || other.type == VariableType::String){
        throw std::runtime_error("Cannot apply '^' to strings");
    }
    else if (type == VariableType::Float || other.type == VariableType::Float){
        throw std::runtime_error("Cannot apply '^' to floats");
    }
    else{
        return Variable::fromInt(this->toInt() ^ other.toInt());
    }
}

bool Variable::operator==(const Variable& other) const{
    return this->toBool() == other.toBool();
}

bool Variable::operator!=(const Variable& other) const{
    return this->toBool() != other.toBool();
}

bool Variable::operator<(const Variable& other) const{
    return this->toBool() < other.toBool();
}

bool Variable::operator<=(const Variable& other) const{
    return this->toBool() <= other.toBool();
}

bool Variable::operator>(const Variable& other) const{
    return this->toBool() > other.toBool();
}

bool Variable::operator>=(const Variable& other) const{
    return this->toBool() >= other.toBool();
}

int16_t Variable::toBool() const{
    switch (type){
        case VariableType::Integer: return intValue != 0 ? -1 : 0;
        case VariableType::Float:   return floatValue != 0.0f ? -1 : 0;
        case VariableType::String:  return !stringValue.empty() ? -1 : 0;
        default: return 0;
    }
}

std::string Variable::toString() const{
    switch (type){
        case VariableType::Integer: return std::to_string(intValue);
        case VariableType::Float:   return std::to_string(floatValue);
        case VariableType::String:  return stringValue;
        default: return "";
    }
}

int16_t Variable::toInt() const{
    switch (type){
        case VariableType::Integer: return intValue;
        case VariableType::Float:   return static_cast<int16_t>(floatValue);
        case VariableType::String:{
            try{
                return static_cast<int16_t>(std::stoi(stringValue));
            } catch (...){
                throw std::runtime_error("Invalid string to int conversion");
            }
        }
        default:
            throw std::runtime_error("Unsupported type for toInt()");
    }
}

float Variable::toFloat() const{
    switch (type){
        case VariableType::Integer: return static_cast<float>(intValue);
        case VariableType::Float:   return floatValue;
        case VariableType::String:{
            try{
                return static_cast<int16_t>(std::stof(stringValue));
            } catch (...){
                throw std::runtime_error("Invalid string to float conversion");
            }
        }
        default:
            throw std::runtime_error("Unsupported type for toFloat()");
    }
}