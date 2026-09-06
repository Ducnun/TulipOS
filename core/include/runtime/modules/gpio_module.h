#pragma once

#ifdef ESP32
#include "Arduino.h"
#endif

#include <stdexcept>
#include "ibasic_module.h"
#include "ast/statement.h"
#include "runtime/core/runtime_context.h"

class GpioModule : public IBasicModule{
public:
    GpioModule(RuntimeContext& ctx) : _ctx(ctx) {}
    ModuleResult call(const std::string& func, const std::vector<Value>& args) override;

private:
    RuntimeContext& _ctx;
    Value write(const std::vector<Value>& args);
    Value read(const std::vector<Value>& args);
};
