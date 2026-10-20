#pragma once

#include "IBasicModule.h"
#include "ast/statement.h"
#include "runtime/core/runtime_context.h"
#include "math.h"

class MathModule : public IBasicModule{
public:
    MathModule() {};
    ModuleResult call(util::StringView func, util::span<const Value> args) override;

private:
    Value basicCastInt(util::span<const Value> args);
};