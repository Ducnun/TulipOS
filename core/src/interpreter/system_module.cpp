#include "runtime/modules/system_module.h"

ModuleResult SystemModule::call(const std::string& func, const std::vector<Value>& args){
    if (func == "DELAY"){ 
        return {CallResult::Success, SystemModule::basicDelay(args)};
    }
    else if(func == "MILLIS"){ 
        return {CallResult::Success, SystemModule::basicMillis(args)};
    }
    return {CallResult::NotHandled, Value::Bool(false)};
}

Value SystemModule::basicDelay(const std::vector<Value>& args){
    if (args.size() > 1 || args[0].type != ValueType::Number) return Value::Bool(false);
    _ctx.host.delay(args[0].number);
    return Value::Bool(true);
}

Value SystemModule::basicMillis(const std::vector<Value>& args){
    if (args.size() > 0) return Value::Bool(false);
    return Value::Float(_ctx.host.millis());
}