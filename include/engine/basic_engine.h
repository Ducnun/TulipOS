#include "tokenizer/tokenizer.h"
#include "parser/parser.h"
#include "interpreter/interpreter.h"
#include "interpreter/pretty_printer.h"
#include "program.h"
#include <sstream>

#define PROGRAM_SIZE    65536
#define PERSISTANT_SIZE 8192
#define SCRATCHPAD_SIZE 4096
#define TOTAL_VARIABLES 512

class BasicEngine {
public:
    BasicEngine(IHost& host) 
        : _host(host),
          _staticArena(PROGRAM_SIZE),
          _symTable(_staticArena),
          _parser(_symTable, _staticArena),
          _ctx(_host, TOTAL_VARIABLES, PERSISTANT_SIZE, SCRATCHPAD_SIZE),
          _interpreter(_ctx, _symTable, _staticArena)
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
    IHost& _host;
    Arena _staticArena;
    SymbolTable _symTable;
    RuntimeContext _ctx;
    Program _program;
    Tokenizer _tokenizer;
    Parser _parser;
    Interpreter _interpreter;

    StmtPtr wrapStatements(std::vector<StmtPtr>&& statements);
    void runInternal(int startLine, bool shouldReset, bool limitToOneLine);
};