#include "tokenizer/tokenizer.h"
#include "parser/parser.h"
#include "interpreter/interpreter.h"
#include "program.h"

class BasicEngine {
public:
    BasicEngine(IOInterface& io) : _io(io), _interpreter(_ctx, io) {}

    void storeLine(int lineNumber, const std::string& source);
    void removeLine(int lineNumber);
    void executeImmediate(const std::string& source);
    void run(void);
    void list(void);
    void clear(void);

private:
    IOInterface& _io;
    Program _program;
    RuntimeContext _ctx;
    Tokenizer _tokenizer;
    Parser _parser;
    Interpreter _interpreter;
};