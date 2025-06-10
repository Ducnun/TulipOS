#include "expression_evaluator.h"

int ExpressionEvaluator::precedence(const std::string& op){
    static std::unordered_map<std::string, int> precedence_map{
        {"=", 0}, {"<>", 0}, {"<", 0}, {"<=", 0}, {">", 0}, {">=", 0},    // Boolean operators
        {"AND", 2}, {"OR", 1},                                            // Logical operators
        {"+", 3}, {"-", 3}, {"*", 4}, {"/", 4}, {"MOD", 4}, {"^", 5}      // Numerical operators
    };

    return precedence_map.count(op) ? precedence_map[op] : 0;
}

bool ExpressionEvaluator::isOperator(const Token& token){
    return token.type == TokenType::Symbol && 
        (token.value == "+" || token.value == "-" || token.value == "/" || token.value == "*" || 
         token.value == "=" || token.value == "<>" || token.value == "<" || token.value == "<=" ||
         token.value == ">" || token.value == ">=" || token.value == "AND" || token.value == "OR" ||
         token.value == "^" || token.value == "MOD");
}

std::vector<Token> ExpressionEvaluator::toRPN(const std::vector<Token>& tokens){
    std::deque<Token> holding;
    std::vector<Token> out;

    for (const Token token : tokens){
        switch (token.type) {
            case TokenType::Float:
            case TokenType::Integer:
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
                    throw std::runtime_error("symbol: " + token.value);
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

EvalResult ExpressionEvaluator::evaluateRPN(const std::vector<Token>& rpnTokens){
    std::vector<std::unique_ptr<Variable>> stack;
    
    // Anonymous function to pop from stack
    auto pop = [&]() -> std::unique_ptr<Variable>{
        if (stack.empty()) throw std::runtime_error("Stack underflow");
        std::unique_ptr<Variable> val = std::move(stack.back());
        stack.pop_back();
        return val;
    };
    
    for (const Token& token : rpnTokens){
        switch (token.type){
            case TokenType::Integer:
                stack.push_back(std::unique_ptr<IntVariable>(new IntVariable(std::stoi(token.value))));
                break;
            case TokenType::Float:
                stack.push_back(std::unique_ptr<FloatVariable>(new FloatVariable(std::stof(token.value))));
                break;
            case TokenType::String:
                stack.push_back(std::unique_ptr<StringVariable>(new StringVariable(token.value)));
                break;
            case TokenType::Identifier:{
                auto it = _variables.find(token.value);
                if (it == _variables.end()){
                    return EvalResult::failure("UNKNOWN VARIABLE: " + token.value);
                }
                stack.push_back(it->second->clone());
                break;
            }
            case TokenType::Symbol:{
                auto right = pop();
                auto left = pop();
                std::unique_ptr<Variable> result;

                if (token.value == "+"){
                    if (left->getType() == VariableType::String || right->getType() == VariableType::String){
                        result = std::unique_ptr<Variable>(new StringVariable(left->toString() + right->toString()));
                    }
                    else if (left->getType() == VariableType::Float || right->getType() == VariableType::Float){
                        result = std::unique_ptr<Variable>(new FloatVariable(std::stof(left->toString()) + std::stof(right->toString())));
                    }
                    else{
                        result = std::unique_ptr<Variable>(new IntVariable(std::stoi(left->toString()) + std::stoi(right->toString())));
                    }
                }
                else if (token.value == "-"){
                    if (left->getType() == VariableType::Float || right->getType() == VariableType::Float){
                        result = std::unique_ptr<Variable>(new FloatVariable(std::stof(left->toString()) - std::stof(right->toString())));
                    }
                    else{
                        result = std::unique_ptr<Variable>(new IntVariable(std::stoi(left->toString()) - std::stoi(right->toString())));
                    }
                }
                else if (token.value == "*"){
                    if (left->getType() == VariableType::Float || right->getType() == VariableType::Float){
                        result = std::unique_ptr<Variable>(new FloatVariable(std::stof(left->toString()) * std::stof(right->toString())));
                    }
                    else{
                        result = std::unique_ptr<Variable>(new IntVariable(std::stoi(left->toString()) * std::stoi(right->toString())));
                    }
                }
                else if (token.value == "/"){
                    if (right->toString() == "0") return EvalResult::failure("ERROR 28 DIVISION BY ZERO");
                    if (left->getType() == VariableType::Float || right->getType() == VariableType::Float){
                        result = std::unique_ptr<Variable>(new FloatVariable(std::stof(left->toString()) / std::stof(right->toString())));
                    }
                    else{
                        result = std::unique_ptr<Variable>(new IntVariable(std::stoi(left->toString()) / std::stoi(right->toString())));
                    }
                }
                else if (token.value == "MOD"){
                    if (left->getType() == VariableType::Float || right->getType() == VariableType::Float){
                        throw std::runtime_error("?MOD ONLY SUPPORTED ON INTS");
                    }
                    else{
                        result = std::unique_ptr<Variable>(new IntVariable(std::stoi(left->toString()) % std::stoi(right->toString())));
                    }
                }
                else if (token.value == "^"){
                    if (left->getType() == VariableType::Float || right ->getType() == VariableType::Float){
                        result = std::unique_ptr<Variable>(new FloatVariable(std::pow(std::stof(left->toString()), std::stof(right->toString()))));
                    }
                    else{
                        result = std::unique_ptr<Variable>(new IntVariable(std::pow(std::stoi(left->toString()), std::stoi(right->toString()))));
                    }
                }
                else if (token.value == "="){
                    result = std::unique_ptr<Variable>(new IntVariable(left->toString() == right->toString() ? -1 : 0));
                }
                else if (token.value == "<>"){
                    result = std::unique_ptr<Variable>(new IntVariable(left->toString() != right->toString() ? -1 : 0));
                }
                else if (token.value == "<"){
                    result = std::unique_ptr<Variable>(new IntVariable(left->toString() < right->toString() ? -1 : 0));
                }
                else if (token.value == "<="){
                    result = std::unique_ptr<Variable>(new IntVariable(left->toString() <= right->toString() ? -1 : 0));
                }
                else if (token.value == ">"){
                    result = std::unique_ptr<Variable>(new IntVariable(left->toString() > right->toString() ? -1 : 0));
                }
                else if (token.value == ">="){
                    result = std::unique_ptr<Variable>(new IntVariable(left->toString() >= right->toString() ? -1 : 0));
                }
                else if (token.value == "AND"){
                    result = std::unique_ptr<Variable>(new IntVariable(left->toBool() & right->toBool()));
                }
                else if (token.value == "OR"){
                    result = std::unique_ptr<Variable>(new IntVariable(left->toBool() | right->toBool()));
                }
                else {
                    throw std::runtime_error("Unknown operator: " + token.value);
                }
                stack.push_back(std::move(result));
                break;
            }
        }
    }

    if (stack.size() != 1){
        return EvalResult::failure("Unexpected token type in expression: " + rpnTokens.front().toString());
    }

    return EvalResult::success(std::move(stack.back()));
}

EvalResult ExpressionEvaluator::evaluate(const std::vector<Token>& tokens){
    try {
        std::vector<Token> rpnTokens = toRPN(tokens);
        return evaluateRPN(rpnTokens);
    }
    catch (const std::exception& ex){
        return EvalResult::failure(std::string("EXCEPTION: ") + ex.what());
    }
}