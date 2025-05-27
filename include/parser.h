#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "tokens.h"
#include "variables.h"
#include "expression_evaluator.h"

enum class StatementType{
    Assignment,
    Print,
    IfThen,
    Goto,
    Input,
    For,
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

    // Print
    std::vector<Token> printTokens;

    // If-Then
    std::vector<Token> conditionTokens;
    std::vector<Token> thenTokens;

    // For loops
    std::vector<Token> forInitTokens;
    std::vector<Token> forToTokens;
    std::vector<Token> forStepTokens;

    // Multipurpose
    std::vector<Token> expressionTokens;    // Assignment, Goto
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
    ParsedStatement parseFor();
};