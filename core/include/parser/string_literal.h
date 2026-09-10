#pragma once

#include "ast/expression.h"

struct StringLiteral : Expr {
    const char* data;
    size_t length;
};