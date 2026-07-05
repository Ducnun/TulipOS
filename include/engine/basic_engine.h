#include "tokenizer/tokenizer.h"
#include "parser/parser.h"
#include "interpreter/interpreter.h"
#include "interpreter/pretty_printer.h"
#include "program.h"
#include <sstream>

#define PROGRAM_SIZE    65536
#define PERSISTANT_SIZE 8192
#define SCRATCHPAD_SIZE 4096

class BasicEngine {
public:
    BasicEngine(IOInterface& io) 
        : _io(io),
          _staticArena(PROGRAM_SIZE),
          _stringRegistry(_staticArena),
          _parser(_stringRegistry, _staticArena),
          _ctx(PERSISTANT_SIZE, SCRATCHPAD_SIZE),
          _interpreter(_ctx, _io, _stringRegistry, _staticArena)
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
    const uint32_t reportProgramSize(){ return _staticArena.getOffset(); }

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