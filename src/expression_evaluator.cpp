#include "expression_evaluator.h"
#include <iostream>

ExpressionEvaluator::ExpressionEvaluator(const std::unordered_map<std::string, VariableValue>& variables)
    : _variables(variables) {}

int ExpressionEvaluator::precedence(const std::string& op){
    static std::unordered_map<std::string, int> precedence_map{
        {"+", 1}, {"-", 2}, {"*", 3}, {"/", 4}
    };

    return precedence_map.count(op) ? precedence_map[op] : 0;
}

bool ExpressionEvaluator::isOperator(const Token& token){
    return token.type == TokenType::Symbol && (token.value == "+" || token.value == "-" || token.value == "/" || token.value == "*");
}

std::vector<Token> ExpressionEvaluator::toRPN(const std::vector<Token>& tokens){
    std::deque<Token> holding;
    std::vector<Token> out;

    for (const Token token : tokens){
        switch (token.type) {
            case TokenType::Number:
            case TokenType::Identifier:
            case TokenType::String:
                out.push_back(token);
                break;
            case TokenType::Symbol:
                if (isOperator(token)) {
                    while (!holding.empty()) {
                        const Token& top = holding.back();
                        if (isOperator(top) && precedence(token.value) <= precedence(top.value)){
                            out.push_back(top);
                            holding.pop_back();
                        } 
                        else {
                            break;
                        }
                    }
                    holding.push_back(token);
                }
                else if (token.value == "("){
                    holding.push_back(token);
                }
                else if (token.value == ")"){
                    bool foundLeft = false;
                    while (!holding.empty()){
                        const Token& top = holding.back();
                        if (top.value == "("){
                            foundLeft = true;
                            holding.pop_back();
                            break;
                        }
                        else {
                            out.push_back(top);
                            holding.pop_back();
                        }
                    }
                    if (!foundLeft){
                        throw std::runtime_error("Mismatched parentheses");
                    }
                }
                else {
                    throw std::runtime_error("Unknown symbol: " + token.value);
                }
                break;
            default:
                throw std::runtime_error("Unsupported token type in expression: " + token.toString());
        }
    }
    
    while (!holding.empty()){
        const Token& top = holding.back();
        if (top.value == "(" || top.value == ")") throw std::runtime_error("Mismatched parentheses at end");
        out.push_back(top);
        holding.pop_back();
    }
    
    return out;
}

VariableValue ExpressionEvaluator::evaluateRPN(const std::vector<Token>& rpnTokens){
    std::vector<VariableValue> stack;

    // Anonymous function to pop from stack
    auto pop = [&]() -> VariableValue{
        if (stack.empty()) throw std::runtime_error("Stack underflow");
        VariableValue val = stack.back();
        stack.pop_back();
        return val;
    };

    for (const Token& token : rpnTokens){
        if (token.type == TokenType::Number){
            // Try float, fallback to int
            if (token.value.find('.') != std::string::npos){
                stack.push_back(VariableValue::fromFloat(std::stof(token.value)));
            }
            else {
                stack.push_back(VariableValue::fromInt(std::stoi(token.value)));
            }
        }
        else if (token.type == TokenType::Identifier){
            auto it = _variables.find(token.value);
            if (it == _variables.end()){
                throw std::runtime_error("Undefined variable: " + token.value);
            }
            stack.push_back(it->second);
        }
        else if (token.type == TokenType::String){
            stack.push_back(VariableValue::fromString(token.value));
        }
        else if (isOperator(token)){
            VariableValue b = pop();
            VariableValue a = pop();

            VariableValue result;

            bool isFloat = (a.type == ValueType::Float || b.type == ValueType::Float);

            float left = (a.type == ValueType::Float) ? a.floatValue : static_cast<float>(a.intValue);
            float right = (b.type == ValueType::Float) ? b.floatValue : static_cast<float>(b.intValue);

            if (token.value == "+"){
                if (a.type == ValueType::String || b.type == ValueType::String){
                    result = VariableValue::fromString(a.toString() + b.toString());
                }
                else{
                    result = isFloat ? VariableValue::fromFloat(left + right) : VariableValue::fromInt(left + right);
                }
            }
            else if (token.value == "-"){
                result = isFloat ? VariableValue::fromFloat(left - right) : VariableValue::fromInt(left - right);
            }
            else if (token.value == "*"){
                result = isFloat ? VariableValue::fromFloat(left * right) : VariableValue::fromInt(left * right);
            }
            else if (token.value == "/"){
                if (right == 0.0f) throw std::runtime_error("Division by zero");
                result = isFloat ? VariableValue::fromFloat(left / right) : VariableValue::fromInt(left / right);
            }
            else {
                throw std::runtime_error("Unknown operator: " + token.value);
            }
            stack.push_back(result);
        }
        else {
            throw std::runtime_error("Unexpected Token: " + token.toString());
        }
    }

    if (stack.size() != 1){
        throw std::runtime_error("Invalid RPN expression, leftover values on stack");
    }

    return stack.back();
}

VariableValue ExpressionEvaluator::evaluate(const std::vector<Token>& tokens){
    std::vector<Token> rpnTokens = toRPN(tokens);
    return evaluateRPN(rpnTokens);
}