#include "variables.h"

int16_t Variable::toInt(const Variable* var) {
    if (!var) throw std::runtime_error("Null variable pointer");

    switch (var->getType()) {
        case VariableType::Integer:
            return static_cast<const IntVariable*>(var)->value;
        case VariableType::Float:
            return static_cast<int16_t>(static_cast<const FloatVariable*>(var)->value);
        default:
            throw std::runtime_error("Cannot convert variable to int");
    }
}
