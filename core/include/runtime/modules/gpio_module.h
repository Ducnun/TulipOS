#pragma once

#ifdef ESP32
#include "Arduino.h"
#endif

#include "IBasicModule.h"
#include "ast/statement.h"
#include "runtime/core/runtime_context.h"

class GpioModule : public IBasicModule{
public:
    GpioModule(RuntimeContext& ctx) : _ctx(ctx) {}
    ModuleResult call(util::StringView func, util::span<const Value> args) override;

private:
    RuntimeContext& _ctx;
    Value write(util::span<const Value> args);
    Value read(util::span<const Value> args);
};
