#include "runtime/modules/math_module.h"

ModuleResult MathModule::call(util::StringView func, util::span<const Value> args){
    if (func == "INT"){
        return {CallResult::Success, MathModule::basicCastInt(args)};
    }
    return {CallResult::NotHandled, Value::Bool(false)};
}

Value MathModule::basicCastInt(util::span<const Value> args){
    if (args.size() != 1) return Value::Bool(false);
    if (args[0].type == ValueType::Number){
        return Value::Float(floor(args[0].number));
    }
    return Value::Bool(false);
}