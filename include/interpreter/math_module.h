#pragma once

#include "ast/statement.h"
#include "value.h"
#include "math.h"

namespace math{
    Value call(const std::string& func, const std::vector<Value>& args);
    Value basicCastInt(const std::vector<Value>& args);
}