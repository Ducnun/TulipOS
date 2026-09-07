#pragma once 

#include <map>
#include <unordered_map>
#include <memory>
#include <sstream>
#include "engine/basic_engine.h"
#include "kernel/kernel.h"
#include "hal/system_config.h"

// Debug
#include "interpreter/pretty_printer.h" 

// TODO: Change to system config struct that can be passed down
#define PROGRAM_SIZE    65535
#define TRANSIENT_SIZE  8192
#define PERSISTENT_SIZE 16384
#define SYMBOL_SIZE     2048

class Shell{
public: 
    /**
     * @brief Construct a new Basic Interpreter object
     * @param kernel Reference to the kernel that created this shell instance
     * Importantly, the engine only sees the ISystemAPI interface, not the full class
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

    void launchEngine();
    
private:
    Kernel& _kernel;            ///< Kernel that started this shell
    BasicEngine* _engine;       ///< Runs BASIC interpreter
    const char* _prompt;        ///< String used as prompt

    // Engine arenas
    Arena _progArena;
    Arena _tranArena;
    Arena _symArena;
    ManagedArena _persArena;

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

    // Temporary stub, TODO: Replace by injecting through IHost/Kernel/SystemAPI?
    SystemConfig sysconfig = {
        .sourceSize = PROGRAM_SIZE,
        .codeSize = TRANSIENT_SIZE,
        .transientSize = TRANSIENT_SIZE,
        .persistentSize = PERSISTENT_SIZE,
        .symbolSize = SYMBOL_SIZE
    };
};
