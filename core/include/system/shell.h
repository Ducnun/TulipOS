#pragma once 

#include <map>
#include <unordered_map>
#include <memory>
#include <sstream>
#include "engine/basic_engine.h"
#include "kernel/kernel.h"

// Debug
#include "interpreter/pretty_printer.h" 

class Shell{
public: 
    /**
     * @brief Construct a new Basic Interpreter object
     * @param kernel Reference to the kernel that created this shell instance
     */
    Shell(Kernel& kernel) : _kernel(kernel) {}

    /**
     * @brief Destroy the Basic Interpreter object
     */
    ~Shell() = default;
    /**
     * @brief Start main REPL loop
     */
    void runREPL();
    
private:
    Kernel& _kernel;
    BasicEngine _engine;        ///< Runs BASIC interpreter
    const char* _prompt;        ///< String used as prompt

    void printWelcome();

    /**
     * @brief Preprocesses the input line, trims whitespace forces all uper
     * @param line 
     * @return std::string 
     */
    std::string toUpper(const std::string& line);

    /**
     * @brief Handled interpreter specific commands 'RUN', 'LIST', 'NEW', 'DUMP'
     * @param line Preprocessed line
     * @return true If the directive was handled
     * @return false If the input needs to be passed down to the rest of the interpreter chain
     */
    bool handleREPLDirective(const std::string& line);
    void handleLoad(const std::string& path);
};
