#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "tokens.h"
#include "variable_types.h"
#include "expression_evaluator.h"

enum class StatementType{
    Assignment,
    Print,
    IfThen,
    Goto,
    Input,
    List,
    Run,
    New,
    Clear,
    ProgramLine,
    NoOp,
    End
};

struct ParsedStatement{
    StatementType type = StatementType::NoOp;
    std::string identifier;

    std::vector<Token> expressionTokens;
    std::vector<Token> printTokens;
    std::vector<Token> conditionTokens;
    std::vector<Token> thenTokens;
    std::vector<Token> identifierTokens;

    int lineNumber = -1;
};

class Parser{
public: 
    Parser(std::vector<Token> tokens) : _tokens(tokens){};
    ~Parser() = default;

    ParsedStatement parse(void);

private:
    std::vector<Token> _tokens;
    ParsedStatement parsePrint();
    ParsedStatement parseLet();
    ParsedStatement parseIfThen();
    ParsedStatement parseGoto();
    ParsedStatement parseInput();
};