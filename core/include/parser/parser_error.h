#pragma once

#include <cstdint>
#include "tokenizer/token.h"

enum class ParseErrorCode : uint8_t {
    None,
    UnexpectedToken,
    ExpectedExpression,
    TooManyItems,
    OutOfMemory,
    SymbolTableFull
};

struct ParseError {
    ParseErrorCode code = ParseErrorCode::None;
    Token token;
    const char* message = nullptr;

    ParseError() = default;

    explicit operator bool() const {
        return code != ParseErrorCode::None;
    }
};