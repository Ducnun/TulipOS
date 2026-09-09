#pragma once

#include "kernel/system_api.h"
#include "runtime/core/runtime_resources.h"
#include "runtime/core/runtime_context.h"
#include "runtime/core/symbol_table.h"
#include "tokenizer/tokenizer.h"
#include "parser/parser.h"
#include "program.h"
#include "interpreter/interpreter.h"
#include "interpreter/pretty_printer.h"

class BasicEngine {
public:
    /**
     * @brief Construct the Engine. 
     * @param sys Access to OS services (Console, Storage, etc)
     * @param res The pre-allocated memory sandbox (moved from Shell)
     */
    BasicEngine(ISystemAPI& sys, RuntimeResources&& res) :   
        _sys(sys),
        _res(std::move(res)),         // Take ownership of the sandbox
        _ctx(_res),                   // Context views the sandbox
        _symTable(_res.symArena, 128), // Pass the arena and capacity
        _parser(_symTable), // Parser uses Program Arena for AST
        _interpreter(_ctx, _symTable, _sys)      // Interpreter executes via Context and System API
    {}

    // --- Program Management ---
    void storeLine(int lineNumber, const std::string& source);
    void removeLine(int lineNumber);
    void loadProgram(const std::string& path);
    void clear(); // Resets both program and memory state

    // --- Execution ---
    void processRawLine(const std::string& line);
    void executeImmediate(const std::string& source);
    void run();
    
    // --- Introspection ---
    void list();
    void DumpProgram();
    void reportParserError();
    
    /**
     * @brief Reports memory usage from the Program Arena
     */
    uint32_t reportProgramSize() { 
        return (uint32_t)_res.codeArena.getOffset(); 
    }

private:
    // 1. External Interfaces
    ISystemAPI& _sys;
    RuntimeResources _res;

    // 2. Execution State
    RuntimeContext _ctx;
    SymbolTable _symTable;

    // 3. Logic Components
    Tokenizer _tokenizer;
    Program _program;
    Parser _parser;
    Interpreter _interpreter;

    // Internal execution helper
    void runInternal(int startLine, bool shouldReset, bool limitToOneLine);
    
    // Helper to turn multiple immediate statements into a single executable block
    // StmtPtr is likely a unique_ptr or a custom pointer type
    StmtPtr wrapStatements(std::vector<StmtPtr>&& statements);
};