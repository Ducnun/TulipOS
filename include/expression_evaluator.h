#pragma once

#include <vector>
#include <deque>
#include <unordered_map>
#include <variable_types.h>
#include "tokens.h"

struct EvalResult{
    VariableValue value;
    bool error = false;
    std::string errorMsg;

    EvalResult(const VariableValue& val, bool err, const std::string& msg)
        : value(val), error(err), errorMsg(msg) {}

    static EvalResult success(const VariableValue& val){
        return EvalResult(val, false, "");    
    }
    static EvalResult failure(const std::string& msg){
        return EvalResult(VariableValue(), true, msg);
    }
};

class ExpressionEvaluator{
public: 
    ExpressionEvaluator(const std::unordered_map<std::string, VariableValue>& variables);
    EvalResult evaluate(const std::vector<Token>& expressionTokens);
    
private:
    const std::unordered_map<std::string, VariableValue> _variables;
    
    std::vector<Token> toRPN(const std::vector<Token>& tokens);
    EvalResult evaluateRPN(const std::vector<Token>& tokens);
    int precedence(const std::string& op);
    bool isOperator(const Token& token);

};