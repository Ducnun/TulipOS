#pragma once

#include "runtime/core/value.h"
#include "utils/span.h"
#include "utils/stringview.h"

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
    virtual ModuleResult call(util::StringView func, util::span<const Value> args) = 0;
};