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
    List,
    Run,
    New,
    Clear,
    ProgramLine,
    NoOp
};

struct ParsedStatement{
    StatementType type = StatementType::NoOp;
    std::string identifier;

    std::vector<Token> expressionTokens;
    std::vector<Token> printTokens;
    std::vector<Token> conditionTokens;
    std::vector<Token> thenTokens;

    int lineNumber = -1;
};

class Parser{
public: 
    Parser(std::vector<Token> tokens, std::unordered_map<std::string, VariableValue>& variables) : _tokens(tokens), _variables(variables){};
    ~Parser() = default;

    ParsedStatement parse(void);

private:
    std::vector<Token> _tokens;
    std::unordered_map<std::string, VariableValue>& _variables;
};