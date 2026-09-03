#pragma once

#include "IBasicModule.h"
#include "ast/statement.h"
#include "runtime/core/runtime_context.h"
#include "math.h"

class MathModule : public IBasicModule{
public:
    MathModule() {};
    ModuleResult call(const std::string& func, const std::vector<Value>& args) override;

private:
    Value basicCastInt(const std::vector<Value>& args);
};