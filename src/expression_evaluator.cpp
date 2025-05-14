#include "expression_evaluator.h"

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
