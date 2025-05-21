#pragma once

#include <vector>
#include <deque>
#include <unordered_map>
#include <variables.h>
#include <memory>
#include "tokens.h"


struct EvalResult {
    std::unique_ptr<const Variable> value;
    bool error = false;
    std::string errorMsg;

    EvalResult(std::unique_ptr<const Variable> val, bool err, const std::string& msg)
        : value(std::move(val)), error(err), errorMsg(msg) {}

    static EvalResult success(std::unique_ptr<const Variable> val){
        return EvalResult(std::move(val), false, "");
    }

    static EvalResult failure(const std::string& msg){
        return EvalResult(nullptr, true, msg);
    }
};

class ExpressionEvaluator{
public: 
    ExpressionEvaluator(const std::unordered_map<std::string, std::unique_ptr<Variable>>& variables)
        : _variables(variables) {}
    EvalResult evaluate(const std::vector<Token>& expressionTokens);
    
private:
    const std::unordered_map<std::string, std::unique_ptr<Variable>>& _variables;
    
    std::vector<Token> toRPN(const std::vector<Token>& tokens);
    EvalResult evaluateRPN(const std::vector<Token>& tokens);
    int precedence(const std::string& op);
    bool isOperator(const Token& token);

};