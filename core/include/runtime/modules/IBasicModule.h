#pragma once

#include "runtime/core/value.h"

enum class CallResult{
    NotHandled,
    Success,
    Error
};

struct ModuleResult{
    CallResult result;
    Value val;
};

class IBasicModule{
public:
    virtual ~IBasicModule() = default;
    virtual ModuleResult call(const std::string& func, const std::vector<Value>& args) = 0;
};