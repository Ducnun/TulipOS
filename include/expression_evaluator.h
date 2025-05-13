#pragma once

#include <vector>
#include <stack>
#include <queue>
#include <unordered_map>
#include <variable_types.h>
#include "tokens.h"

class ExpressionEvaluator {
public: 
    ExpressionEvaluator(const std::unordered_map<std::string, VariableValue>& variables);
    std::string evaluate(const std::vector<Token>& expressionTokens);

private:
    const std::unordered_map<std::string, VariableValue> _variables;

    std::vector<Token> toRPN(const std::vector<Token>& tokens);
    std::string evaluateRPN(const std::vector<Token>& tokens);
    int precedence(const std::string& op);
    bool isOperator(const Token& token);

};