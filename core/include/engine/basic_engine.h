#include "tokenizer/tokenizer.h"
#include "parser/parser.h"
#include "interpreter/interpreter.h"
#include "interpreter/pretty_printer.h"
#include "kernel/system_api.h"
#include "hal/system_config.h"
#include "program.h"
#include <sstream>

struct RuntimeResources {
    Arena& programArena;
    Arena& transientArena;
    Arena& symbolArena;
    ManagedArena& persistentArena;
};

class BasicEngine {
public:
    BasicEngine(ISystemAPI& sys, RuntimeResources res);

    void storeLine(int lineNumber, const std::string& source);
    void processRawLine(const std::string& line);
    void removeLine(int lineNumber);
    void executeImmediate(const std::string& source);
    void run(void);
    void list(void);
    void clear(void);
    void loadProgram(const std::string& scrip);
    void DumpProgram();
    uint32_t reportProgramSize(){ return _staticArena.getOffset(); }

private:
    ISystemAPI& _sys;
    Arena& _staticArena;
    RuntimeContext _ctx;
    SymbolTable _symTable;
    Parser _parser;
    Program _program;
    Tokenizer _tokenizer;
    Interpreter _interpreter;

    StmtPtr wrapStatements(std::vector<StmtPtr>&& statements);
    void runInternal(int startLine, bool shouldReset, bool limitToOneLine);
};