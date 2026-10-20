#include "runtime/modules/system_module.h"

ModuleResult SystemModule::call(util::StringView func, util::span<const Value> args){
    if (func == "DELAY"){ 
        return {CallResult::Success, SystemModule::basicDelay(args)};
    }
    else if(func == "MILLIS"){ 
        return {CallResult::Success, SystemModule::basicMillis(args)};
    }
    return {CallResult::NotHandled, Value::Bool(false)};
}

Value SystemModule::basicDelay(util::span<const Value> args){
    if (args.size() != 1 || args[0].type != ValueType::Number) return Value::Bool(false);
    _ctx.taskControl.delay(args[0].number);
    return Value::Bool(true);
}

Value SystemModule::basicMillis(util::span<const Value> args){
    if (!args.empty()) return Value::Bool(false);
    return Value::Float(_ctx.taskControl.millis());
}