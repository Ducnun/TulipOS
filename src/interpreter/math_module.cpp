#include "runtime/modules/math_module.h"

Value math::call(const std::string& func, const std::vector<Value>& args){
    if (func == "INT"){
        return math::basicCastInt(args);
    }
    return Value::Bool(false);
}

Value math::basicCastInt(const std::vector<Value>& args){
    if (args.size() > 1) return Value::Bool(false);
    if (args[0].type == ValueType::Number){
        return Value::Float(std::floor(args[0].number));
    }
    return Value::Bool(false);
}