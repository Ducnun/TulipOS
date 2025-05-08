#pragma once 

#include <map>
#include <unordered_map>
#include "io_interface.h"
#include "tokenizer.h"
#include "parser.h"

class BasicInterpreter{
public: 
        BasicInterpreter(IOInterface& io);
        ~BasicInterpreter() = default;
        void runREPL();

private:
    IOInterface& _io;
    std::map<int, std::string> programLines;
    std::unordered_map<std::string, int> _variables;

    std::string preprocess(const std::string& line);
    void storeProgramLine(const std::string& line);
    void evaluateImmediateCommand(const std::string& line);
    void runProgram();
    void listProgram();
    void clearProgram();
};
