#pragma once

#include <map>
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
    std::string output = "";
};

class Executor{
public:
    Executor(IOInterface& io, std::unordered_map<std::string, VariableValue>& variables): _io(io), _variables(variables){}

    ExecutionResult execute(const ParsedStatement&);

private:
    IOInterface& _io;
    std::unordered_map<std::string, VariableValue>& _variables;
};
