#pragma once

#ifdef ESP32
#include "Arduino.h"
#endif

#include "ast/statement.h"
#include "value.h"

namespace gpio{
    Value call(const CallStmt& stmt, const std::vector<Value>& args);
    Value write(const std::vector<Value>& args);
    Value read(const std::vector<Value>& args);
};
