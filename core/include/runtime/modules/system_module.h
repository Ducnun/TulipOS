#pragma once

#ifdef ESP32
#include <Arduino.h>
#else
#include <chrono>
#endif

#include "ibasic_module.h"
#include "ast/statement.h"
#include "runtime/core/runtime_context.h"

class SystemModule : public IBasicModule{
public:
    SystemModule(RuntimeContext& ctx) : _ctx(ctx) {};
    ModuleResult call(const std::string& funct, const std::vector<Value>& args) override;
private:
    RuntimeContext& _ctx;

    Value basicDelay(const std::vector<Value>& args);
    Value basicMillis(const std::vector<Value>& args);
};