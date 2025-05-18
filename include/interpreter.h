#pragma once 

#include <map>
#include <unordered_map>
#include <sstream>
#include "io_interface.h"
#include "tokenizer.h"
#include "parser.h"
#include "variable_types.h"
#include "executor.h"
#include "program.h"

class BasicInterpreter{
public: 
    BasicInterpreter(IOInterface& io);
    ~BasicInterpreter() = default;
    void runREPL();
    
private:
    IOInterface& _io;
    Executor _executor;
    std::unordered_map<std::string, VariableValue> _variables;
    Program _program;

    std::string preprocess(const std::string& line);
    bool handleREPLDirective(const std::string& line);
    void storeProgramLine(const std::string& line);
    void evaluateImmediateCommand(const std::string& line);
    void runProgram();
    void listProgram();
    void clearProgram();
    void clearVariables();
};
