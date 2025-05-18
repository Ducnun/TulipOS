#pragma once

#include <map>
#include "expression_evaluator.h"
#include "io_interface.h"
#include "parser.h"
#include "expression_evaluator.h"

class Executor{
public:
    Executor(IOInterface& io, std::unordered_map<std::string, VariableValue>& variables): _io(io), _variables(variables){}

    void execute(const ParsedStatement&);

private:
    IOInterface& _io;
    std::unordered_map<std::string, VariableValue>& _variables;
};
