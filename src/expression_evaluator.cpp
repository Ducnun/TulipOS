#include "expression_evaluator.h"

int ExpressionEvaluator::precedence(TokenType type)
{
    switch (type)
    {
        case TokenType::Equal:
        case TokenType::NotEqual:
        case TokenType::Less:
        case TokenType::LessEqual:
        case TokenType::Greater:
        case TokenType::GreaterEqual:
            return 0;

        case TokenType::Or:
            return 1;

        case TokenType::And:
            return 2;

        case TokenType::Plus:
        case TokenType::Minus:
            return 3;

        case TokenType::Star:
        case TokenType::Slash:
        case TokenType::Mod:
            return 4;

        case TokenType::Caret:
            return 5;

        default:
            return -1;
    }
}

bool ExpressionEvaluator::isOperator(TokenType type)
{
    switch (type) {
        case TokenType::Plus:
        case TokenType::Minus:
        case TokenType::Star:
        case TokenType::Slash:
        case TokenType::Caret:
        case TokenType::Mod:
        case TokenType::Equal:
        case TokenType::NotEqual:
        case TokenType::Less:
        case TokenType::LessEqual:
        case TokenType::Greater:
        case TokenType::GreaterEqual:
        case TokenType::And:
        case TokenType::Or:
            return true;
        default:
            return false;
    }
}

std::vector<Token> ExpressionEvaluator::toRPN(const std::vector<Token>& tokens){
    std::deque<Token> holding;
    std::vector<Token> out;

    for (const Token& token : tokens) {
    switch (token.type) {
        // Operands
        case TokenType::Integer:
        case TokenType::Float:
        case TokenType::String:
        case TokenType::Identifier: out.push_back(token); break;
        case TokenType::LeftParen: holding.push_back(token); break;
        case TokenType::RightParen:
            while (!holding.empty() &&
                   holding.back().type != TokenType::LeftParen) {
                out.push_back(holding.back());
                holding.pop_back();
            }

            if (holding.empty())
                throw std::runtime_error("Mismatched parentheses");

            holding.pop_back(); // discard '('
            break;

        // Operators
        default:
            if (!isOperator(token.type)) {
                throw std::runtime_error(
                    "Unexpected token in expression: " +
                    TokenUtils::typeToString(token.type));
            }

            while (!holding.empty() &&
                   isOperator(holding.back().type) &&
                   precedence(token.type) <= precedence(holding.back().type)) {
                out.push_back(holding.back());
                holding.pop_back();
            }

            holding.push_back(token);
            break;
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
    std::vector<Variable> stack;
    
    // Anonymous function to pop from stack
    auto pop = [&]() -> Variable{
        if (stack.empty()) throw std::runtime_error("Stack underflow");
        Variable val = stack.back();
        stack.pop_back();
        return val;
    };
    
    for (const Token& token : rpnTokens){
        Variable right = pop();
        Variable left = pop();
        Variable result;

        switch (token.type){
            case TokenType::Integer:
                stack.push_back(Variable::fromInt(std::stoi(token.value)));
                break;
            case TokenType::Float:
                stack.push_back(Variable(std::stof(token.value)));
                break;
            case TokenType::String:
                stack.push_back(Variable(token.value));
                break;
            case TokenType::Identifier:{
                auto it = _variables.find(token.value);
                if (it == _variables.end()){
                    return EvalResult::failure("UNKNOWN VARIABLE: " + token.value);
                }
                stack.push_back(std::move(it->second));
                break;
            }
            case TokenType::Plus:
                    result = left + right; break;
            case TokenType::Minus:
                    result = left - right; break;
            case TokenType::Star:
                    result = left * right; break;
            case TokenType::Slash:
                if (right.toString() == "0")
                    return EvalResult::failure("ERROR 28 DIVISION BY ZERO");
                result = left / right;
                break;
            case TokenType::Mod:
                if (left.type == VariableType::Float || right.type == VariableType::Float) {
                    throw std::runtime_error("?MOD ONLY SUPPORTED ON INTS");
                }
                result = Variable::fromInt(left.intValue % right.intValue);
                break;
            case TokenType::Caret:
                result = left ^ right; break;
            case TokenType::Equal:
                result = (float)(left.stringValue == right.stringValue ? -1 : 0); break;
            case TokenType::NotEqual:
                result = (float)(left.stringValue != right.stringValue ? -1 : 0); break;
            case TokenType::Less:
                result = (float)(left.stringValue < right.stringValue ? -1 : 0); break;
            case TokenType::LessEqual:
                result = (float)(left.stringValue <= right.stringValue ? -1 : 0); break;
            case TokenType::Greater:
                result = (float)(left.stringValue > right.stringValue ? -1 : 0); break;
            case TokenType::GreaterEqual:
                result = (float)(left.stringValue >= right.stringValue ? -1 : 0); break;
            case TokenType::And:
                result = (float)(left.floatValue == -1 && right.floatValue == -1 ? -1 : 0); break;

            case TokenType::Or:
                result = (float)(left.floatValue == -1 || right.floatValue == -1 ? -1 : 0); break;

            default:
                throw std::runtime_error("Unexpected token in RPN evaluation: " +
                                        TokenUtils::typeToString(token.type));
        }
    }

    if (stack.size() != 1){
        return EvalResult::failure("Unexpected token type in expression: " + TokenUtils::typeToString(rpnTokens.front().type));
    }

    return EvalResult::success(pop());
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