#pragma once

#include <math.h>
#include <vector>
#include <deque>
#include <unordered_map>
#include <variables.h>
#include <memory>
#include "tokens.h"


/**
 * @brief Holds the expression evaluator result
 * Returns a unique pointer to a Variable type that helds the evaluated result. Can also have an error flag set and a message included if the evaluation was not succesful
 */
struct EvalResult {
    Variable value;
    bool error = false;
    std::string errorMsg;

    /**
     * @brief Construct a new Eval Result object
     * @param val Value of result Variable
     * @param err Error flag
     * @param msg Error message
     */
    EvalResult(Variable val, bool err, const std::string& msg)
        : value(std::move(val)), error(err), errorMsg(msg) {}

    /**
     * @brief Constructs a succesful evaluation result
     * @param val Holds the variable value
     * @return EvalResult The evaluated result, with error flag set to false
     */
    static EvalResult success(Variable val){
        return EvalResult(std::move(val), false, "");
    }

    /**
     * @brief Constructs a failure evaluation result
     * @param msg Error message
     * @return EvalResult A result with an error flag set to true and an error message. Variable is a null pointer by default
     */
    static EvalResult failure(const std::string& msg){
        return EvalResult(Variable::fromInt(0), true, msg);
    }
};

class ExpressionEvaluator{
public: 
    /**
     * @brief Construct a new Expression Evaluator object
     * @param variables The shared variable map, needed to dereference variables used in expressions
     */
    ExpressionEvaluator(const std::unordered_map<std::string, Variable>& variables)
        : _variables(variables) {}

    /**
     * @brief Evaluates the passed expression
     * @param expressionTokens Vector with all tokens that need to be evaluated (needs to be tokenized properly)
     * @return EvalResult Result of the evaluated expression
     */
    EvalResult evaluate(const std::vector<Token>& expressionTokens);
    
private:
    const std::unordered_map<std::string, Variable>& _variables; ///< Reference to the shared runtime variables
    
    /**
     * @brief Helper function that transforms an inline expression into a reverse polish notation using shunting yard algorithm
     * @param tokens Expression tokens (inline)
     * @return std::vector<Token> Expression tokens (postfix/reverse polish notation)
     */
    std::vector<Token> toRPN(const std::vector<Token>& tokens);

    /**
     * @brief Evaluates a postfix/reverse polish notation expression
     * @param tokens Postfix/reverse polish notiation tokens
     * @return EvalResult Result of the evluation
     */
    EvalResult evaluateRPN(const std::vector<Token>& tokens);

    /**
     * @brief Helper function that returns the precedence of an operator using a predefined map
     * @param op Operation for which precedence is looked up
     * @return int Precedence encoded as int (higher int is higher precendence)
     */
    int precedence(const std::string& op);

    /**
     * @brief Helper function that validates that a symbol token is a valid operator
     * @param token Token object to be checked
     * @return true If Token matches a value in a predefined map
     * @return false If token is not found in the predefined map
     */
    bool isOperator(const Token& token);
};