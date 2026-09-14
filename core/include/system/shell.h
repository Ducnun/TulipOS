// #pragma once 

// #include <map>
// #include <unordered_map>
// #include <memory>
// #include <sstream>
// #include "hal/ihost.h"
// #include "engine/basic_engine.h"
// #include "services/storage_service.h"

// // Debug
// #include "interpreter/pretty_printer.h" 

// class Shell{
// public: 
//     /**
//      * @brief Construct a new Basic Interpreter object
//      * @param io A reference to an IHost
//      */
//     Shell(IHost& host, StorageService& storage);

//     /**
//      * @brief Destroy the Basic Interpreter object
//      */
//     ~Shell() = default;

//     /**
//      * @brief Start main REPL loop
//      */
//     void runREPL();

//     void feed(char c);
    
// private:
//     IHost& _host;   ///< Reference to IO system for input/output
//     StorageService& _storage;   ///< Reference to storage service
//     BasicEngine _engine;        ///< Runs BASIC interpreter
//     const char* _prompt;        ///< String used as prompt
//     std::string _inputBuffer;

//     void printWelcome();

//     /**
//      * @brief Preprocesses the input line, trims whitespace forces all uper
//      * @param line 
//      * @return std::string 
//      */
//     std::string toUpper(const std::string& line);

//     /**
//      * @brief Handled interpreter specific commands 'RUN', 'LIST', 'NEW', 'DUMP'
//      * @param line Preprocessed line
//      * @return true If the directive was handled
//      * @return false If the input needs to be passed down to the rest of the interpreter chain
//      */
//     bool handleREPLDirective(const std::string& line);
//     void handleLoad(const std::string& path);
// };
