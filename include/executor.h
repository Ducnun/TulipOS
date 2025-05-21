#pragma once

#include <map>
#include <functional>
#include <sstream>
#include <cctype>
#include "expression_evaluator.h"
#include "io_interface.h"
#include "parser.h"
#include "expression_evaluator.h"

enum ExecutionAction{
    Continue,
    Jump,
    Halt,
    Wait,
    Error
};

struct ExecutionResult{
    ExecutionAction action = ExecutionAction::Continue;
    int jumpTarget = -1;
    bool error = false;
    std::string output = "";
};

class Executor{
public:
    Executor(IOInterface& io, std::unordered_map<std::string, std::unique_ptr<Variable>>& variables): _io(io), _variables(variables){}

    ExecutionResult execute(const ParsedStatement& stmt);

    // Sub Handlers
    ExecutionResult handlePrint(const ParsedStatement& stmt);
    ExecutionResult handleAssignment(const ParsedStatement& stmt);
    ExecutionResult handleIfThen(const ParsedStatement& stmt);
    ExecutionResult handleGoto(const ParsedStatement& stmt);
    ExecutionResult handleEnd(const ParsedStatement& stmt);
    ExecutionResult handleInput(const ParsedStatement& stmt);

private:
    EvalResult safeEvaluate(const std::vector<Token>& tokens, ExecutionResult& result);

    IOInterface& _io;
    std::unordered_map<std::string, std::unique_ptr<Variable>>& _variables;
};
