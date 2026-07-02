#include "tokenizer/tokenizer.h"
#include "parser/parser.h"
#include "interpreter/interpreter.h"
#include "interpreter/pretty_printer.h"
#include "program.h"
#include <sstream>

class BasicEngine {
public:
    BasicEngine(IOInterface& io) 
        : _io(io),
          _staticArena(1024 * 32),          // 1. Give the Arena a size (e.g., 32KB)
          _stringRegistry(_staticArena),     // 2. Pass Arena to Registry
          _parser(_stringRegistry, _staticArena), // 3. Pass Registry and Arena to Parser
          _ctx(),                           // 4. Default init context
          _interpreter(_ctx, _io, _stringRegistry, _staticArena) // 5. Init interpreter
    {
        // Constructor body
    }

    void storeLine(int lineNumber, const std::string& source);
    void removeLine(int lineNumber);
    void executeImmediate(const std::string& source);
    void run(void);
    void list(void);
    void clear(void);
    void loadProgram(const std::string& scrip);
    void DumpProgram();

private:
    IOInterface& _io;
    Arena _staticArena;
    StringRegistry _stringRegistry;
    RuntimeContext _ctx;
    Program _program;
    Tokenizer _tokenizer;
    Parser _parser;
    Interpreter _interpreter;

    StmtPtr wrapStatements(std::vector<StmtPtr>&& statements);
    void runInternal(int startLine, bool shouldReset, bool limitToOneLine);
};