#pragma once 

#include <map>
#include <unordered_map>
#include <memory>
#include <sstream>
#include "io_interface.h"
#include "tokenizer.h"
#include "parser.h"
#include "variables.h"
#include "executor.h"
#include "program.h"

class BasicInterpreter{
public: 
    /**
     * @brief Construct a new Basic Interpreter object
     * @param io A reference to an IOInterface (console or ESP32 based)
     */
    BasicInterpreter(IOInterface& io);
    /**
     * @brief Destroy the Basic Interpreter object
     */
    ~BasicInterpreter() = default;
    /**
     * @brief Start main REPL loop
     */
    void runREPL();
    
private:
    IOInterface& _io;   ///< Reference to IO system for input/output
    Executor _executor; ///< Executes parsed BASIC statements
    std::unordered_map<std::string, std::unique_ptr<Variable>> _variables;   ///< Current runtime variables map
    Program _program;           ///< Current stored/editing program

    /**
     * @brief Preprocesses the input line, trims whitespace forces all uper
     * @param line 
     * @return std::string 
     */
    std::string preprocess(const std::string& line);

    /**
     * @brief Handled interpreter specific commands 'RUN', 'LIST', 'NEW', 'DUMP'
     * @param line Preprocessed line
     * @return true If the directive was handled
     * @return false If the input needs to be passed down to the rest of the interpreter chain
     */
    bool handleREPLDirective(const std::string& line);

    /**
     * @brief Stores a BASIC line if processed line starts with a line number
     * @param line Full line including line number
     */
    void storeProgramLine(const std::string& line);

    /**
     * @brief Evaluate command in immediate mode (no line number present)
     * @param line 
     */
    void evaluateImmediateCommand(const std::string& line);

    /**
     * @brief Runs currently stored program lines
     */
    void runProgram();

    /**
     * @brief Prints currently stored program
     */
    void listProgram();

    /**
     * @brief Clears currently stored program lines
     */
    void clearProgram();

    /**
     * @brief Clears runtime variables stored in current REPL loop
     */
    void clearVariables();
};
