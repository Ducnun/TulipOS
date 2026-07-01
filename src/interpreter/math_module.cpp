#include "interpreter/math_module.h"

Value math::call(const std::string& func, const std::vector<Value>& args){
    if (func == "INT"){
        return math::basicCastInt(args);
    }
    return false;
}

Value math::basicCastInt(const std::vector<Value>& args){
    if (args.size() > 1) return Value(false);
    return Value(std::floor(args[0].asNumber()));
}