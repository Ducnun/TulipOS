#pragma once

#ifdef ESP32
#include <Arduino.h>
#else
#include <chrono>
#endif

#include "IBasicModule.h"
#include "ast/statement.h"
#include "runtime/core/runtime_context.h"

class SystemModule : public IBasicModule{
public:
    SystemModule(RuntimeContext& ctx) : _ctx(ctx) {};
    ModuleResult call(util::StringView funct, util::span<const Value> args) override;
private:
    RuntimeContext& _ctx;

    Value basicDelay(util::span<const Value> args);
    Value basicMillis(util::span<const Value> args);
};