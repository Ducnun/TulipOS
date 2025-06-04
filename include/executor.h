#pragma once

#include <map>
#include <functional>
#include <sstream>
#include <cctype>
#include <stack>
#include "expression_evaluator.h"
#include "io_interface.h"
#include "parser.h"
#include "expression_evaluator.h"

/**
 * @brief Lists all possible execution actions
 * This helps the currently executing program/command to control the program flow
 * Options are either to continue, jump to a different line (program), halt (program), wait (inputs etc.) or none of the above, which returns an error 
 */
enum ExecutionAction{
    Continue,
    Jump,
    Halt,
    Wait,
    Error
};

struct LoopState{
    std::string varName;
    int16_t current;
    int16_t end;
    int16_t step;
    int lineNumberToReturnTo;
};

/**
 * @brief Holds a formatted result/report of the execution
 * Contains the action needed next as well as secondary needed parameters for an action as well as the raw output
 * Can have error set to true in which case the error message can also be read from output
 */
struct ExecutionResult{
    ExecutionAction action = ExecutionAction::Continue; ///< Next action needed (default = Continue)
    int jumpTarget = -1;    ///< Line to jump to if action is set to jump (default = -1)
    bool error = false; ///< Error flag (default = false)
    std::string output = ""; ///< Raw output string (When error flag is set this holds the error message)
};

class Executor{
public:
    /**
     * @brief Construct a new Executor object
     * @param io 
     * @param variables 
     */
    Executor(IOInterface& io, std::unordered_map<std::string, std::unique_ptr<Variable>>& variables): _io(io), _variables(variables){}

    /**
     * @brief Dispatches the statement to a specific statement handler depending on statement type
     * @param stmt Parsed statement that can be executed
     * @return ExecutionResult Result from the executor
     */
    ExecutionResult execute(const ParsedStatement& stmt);
    
private:
    IOInterface& _io; ///< Reference to interpreters IO interface
    std::unordered_map<std::string, std::unique_ptr<Variable>>& _variables; ///< Reference to shared runtime variables
    std::stack<LoopState> _loopStack;
    std::unordered_set<int> _initializedForLines;

    /**
     * @brief Helper function that checks whether an error is thrown by any of the statement handlers
     * @param tokens Inputs tokens
     * @param result Reference to the result of the statement handler, sets error flag when expression evaluator throws an error
     * @return EvalResult Result of the expression evaluator including flags or errors if needed
     */
    EvalResult safeEvaluate(const std::vector<Token>& tokens, ExecutionResult& result);
    
    // Sub Handlers

    /**
     * @brief Handles statements with StatementType Print, evaluates the input if needed and looks up identifier values
     * @param stmt Print statement
     * @return ExecutionResult Result with continue action if printed succesfuly
     */
    ExecutionResult handlePrint(const ParsedStatement& stmt);

    /**
     * @brief Handles an assignment statement, evaluates and stored the variable
     * @param stmt Assignment/Let statement
     * @return ExecutionResult Result with continue action if assigned succesful
     */
    ExecutionResult handleAssignment(const ParsedStatement& stmt);

    /**
     * @brief Handles an if-then statement, evaluates the if statement and checks whether its true. If so, tokenizes and parses the then statement and sends the then statement back to the dispatcher
     * @param stmt If Then statement
     * @return ExecutionResult Result of the statement after THEN when IF evaluates to true, otherwise returns the default result (action continue)
     */
    ExecutionResult handleIfThen(const ParsedStatement& stmt);

    /**
     * @brief Handles a GoTo statement (program), moves the program counter to the desired line if possible. Evaluates the goto line expression if needed
     * @param stmt GOTO statement
     * @return ExecutionResult Result with jump as action and the target line as jumpTarget datafield
     */
    ExecutionResult handleGoto(const ParsedStatement& stmt);

    /**
     * @brief Handles an END statement
     * @param stmt End statement
     * @return ExecutionResult Result with action Halt
     */
    ExecutionResult handleEnd(const ParsedStatement& stmt);

    /**
     * @brief Handles and input satement. Takes over IO from the interpreter to read inputs and store if valid. Directly declares and defines the variable
     * @param stmt Input stament, including print tokens and identifier tokens
     * @return ExecutionResult Result with action Continue
     */
    ExecutionResult handleInput(const ParsedStatement& stmt);

    ExecutionResult handleFor(const ParsedStatement& stmt);

    ExecutionResult handleNext(const ParsedStatement& stmt);
};
