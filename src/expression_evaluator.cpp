#include "expression_evaluator.h"

ExpressionEvaluator::ExpressionEvaluator(const std::unordered_map<std::string, std::string>& variables)
    : _variables(variables) {}

int ExpressionEvaluator::precedence(const std::string& op){
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    if (op == "+" || op == "-") return 0;
}

