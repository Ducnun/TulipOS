#pragma once 

#include <map>
#include <unordered_map>
#include <memory>
#include <sstream>
#ifdef ESP32    // Heap cap not available for native builds
#include "esp_heap_caps.h"
#endif
#include "host/ihost.h"
#include "engine/basic_engine.h"

// Debug
#include "interpreter/pretty_printer.h" 

class Shell{
public: 
    /**
     * @brief Construct a new Basic Interpreter object
     * @param io A reference to an IHost (console or ESP32 based)
     */
    Shell(IHost& host);
    /**
     * @brief Destroy the Basic Interpreter object
     */
    ~Shell() = default;
    /**
     * @brief Start main REPL loop
     */
    void runREPL();
    
private:
    IHost& _host;   ///< Reference to IO system for input/output
    BasicEngine _engine;

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
};
