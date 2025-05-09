#include "expression_evaluator.h"

ExpressionEvaluator::ExpressionEvaluator(const std::unordered_map<std::string, std::string>& variables)
    : _variables(variables) {}

int ExpressionEvaluator::precedence(const std::string& op){
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    return 0;
}

bool ExpressionEvaluator::isOperator(const Token& token){
    return token.type == TokenType::Symbol && (token.value == "+" || token.value == "-" || token.value == "/" || token.value == "*");
}

std::vector<Token> ExpressionEvaluator::toRPN(const std::vector<Token>& tokens){
    std::vector<Token> output;
    std::stack<Token> ops;


}
