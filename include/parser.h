#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "tokens.h"
#include "variables.h"
#include "expression_evaluator.h"

/**
 * @brief Types of statements
 * Enum that represents the type of statement returned by the parser based on input tokens
 */
enum class StatementType{
    Assignment,
    Print,
    IfThen,
    Goto,
    Input,
    For,
    Next,
    List,
    Run,
    New,
    Clear,
    ProgramLine,
    NoOp,
    End
};

/**
 * @brief Struct that holds the statement type as well as all needed tokens depending on the statement type
 */
struct ParsedStatement{
    StatementType type = StatementType::NoOp;
    
    // Print
    std::vector<Token> printTokens;
    
    // If-Then
    std::vector<Token> conditionTokens;
    std::vector<Token> thenTokens;
    
    // For loops
    std::vector<Token> forInitTokens;
    std::vector<Token> forToTokens;
    std::vector<Token> forStepTokens;
    
    // Multipurpose
    std::vector<Token> expressionTokens;    // Assignment, Goto
    std::vector<Token> identifierTokens;
    std::string identifier;

    int lineNumber = -1;
};

class Parser{
public: 
    /**
     * @brief Construct a new Parser object, done for every parse task so its stateless
     * @param tokens Set of tokens to be parsed
     */
    Parser(std::vector<Token> tokens) : _tokens(tokens){};
    /**
     * @brief Destroy the Parser object
     */
    ~Parser() = default;

    /**
     * @brief Main parse function that can be called on a new Parser object that was passed the tokens during construction
     * @return ParsedStatement A struct containing the statement type as well as all associated tokens
     */
    ParsedStatement parse(void);

private:
    std::vector<Token> _tokens;

    /**
     * @brief Private helper function that handles PRINT statements
     * @return ParsedStatement Holds the statementtype print as well as tokens to be printed.abort
     * These tokens might need to be evaluated first in case it's an expression
     */
    ParsedStatement parsePrint();

    /**
     * @brief Private helper function that handles LET statements
     * @return ParsedStatement Holds the identifier tokens as well as the value tokens that need to be assigned to it
     * These tokens might need to be evaluated first in case it's an expression
     */
    ParsedStatement parseLet();

    /**
     * @brief Private helper function that handles IF THEN statements
     * @return ParsedStatement Holds the tokens for the if statement and the then statement separately
     * "if" needs to be evaluated, in which case the raw "then" tokens can be parsed into their own statement
     */
    ParsedStatement parseIfThen();

    /**
     * @brief Private helper function that handles GOTO statements
     * @return ParsedStatement Holds the tokens for the goto statement in the expression tokens
     * These tokens might need to be evaluated first in case it's an expression
     */
    ParsedStatement parseGoto();

    /**
     * @brief Private helper function that handles INPUT statements
     * @return ParsedStatement Holds the statementtype input as well as the identifier that the input is stored in
     * as well as print tokens if applicable. These tokens might need to be evaluated first in case it's an expression
     */
    ParsedStatement parseInput();

    /**
     * @brief Private helper function that handles FOR statements
     * @return ParsedStatement Holds the statementtype for as well as the init tokens, to tokens and step tokens. All of these can be
     * expressions, in which case they have to be evaluated first
     */
    ParsedStatement parseFor();

    /**
     * @brief Private helper function that handles NEXT statements
     * @return ParsedStatement Holds the statementtype next as well as the identifier associated with the loop
     */
    ParsedStatement parseNext();
};