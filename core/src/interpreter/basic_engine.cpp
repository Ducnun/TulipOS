#include "engine/basic_engine.h"
#include <sstream>

void BasicEngine::storeLine(int lineNumber, const std::string& source){
    // Store line of source code in src Arena and generate string view for it
    char* sMem = (char*)_res.srcArena.alloc(source.length() + 1);
    memcpy(sMem, source.c_str(), source.length() + 1);
    util::StringView sourceView(sMem, source.length());

    _tokenizer.init(sourceView, lineNumber);
    StmtPtr statement = _parser.parseLine(_tokenizer, _res.codeArena);

    if (statement) {
        _program.addLine(lineNumber, statement, sourceView);
    }
}

void BasicEngine::processRawLine(const std::string& line) {
    if (line.empty()) return;

    // Find the first space to separate line number from code
    size_t firstSpace = line.find(' ');
    if (firstSpace != std::string::npos) {
        try {
            int lineNum = std::stoi(line.substr(0, firstSpace));
            std::string content = line.substr(firstSpace + 1);
            storeLine(lineNum, content);
        } catch (...) {
            // Log error: Line doesn't start with a valid number
        }
    }
}

// Removes a line
void BasicEngine::removeLine(int lineNumber) {
    _program.removeLine(lineNumber);
}

void BasicEngine::executeImmediate(const std::string& source){
    if (source.empty()) return;

    // Create source view and pass to tokenizer
    util::StringView sourceView(source.data(), source.length());
    _tokenizer.init(sourceView, 0);

    // Create and check validity of executable AST
    StmtPtr statement = _parser.parseLine(_tokenizer, _res.codeArena);
    if (!statement) {
        reportParserError();
        return;
    }

    // Temp run as line 0, delete after
    _program.addLine(0, statement, sourceView);
    runInternal(0, false, true);
    _program.removeLine(0);
}

void BasicEngine::run() {
    // Normal program execution starts from the beginning and resets state
    // DumpProgram();
    runInternal(_program.begin() != _program.end() ? _program.begin()->first : 1, true, false);
}

// Unified core execution loop
void BasicEngine::runInternal(int startLine, bool shouldReset, bool limitToOneLine){
    if (_program.isEmpty()) return;
    if (shouldReset) _ctx.reset();

    _ctx.halted = false;
    _ctx.jumpRequested = false;
    
    auto pc = _program.lower_bound(startLine);
    
    _sys.system().yield();
    while (pc != _program.end() && !_ctx.halted){
        _ctx.currentLine = pc->first;

        // Handle break requests
        if (_sys.console().checkAndResetBreak()){
            _sys.console().print("! BREAK TRIG'D ON ");
            _sys.console().print(std::to_string(_ctx.currentLine));
            _sys.console().print("\n");
            return;
        }
        _sys.system().yield();
        
        if (pc->second.statement){
            try{
                pc->second.statement->accept(_interpreter);
            } catch (const std::exception& e){
                _sys.console().print("? RUNTIME ERROR: " + std::string(e.what()) + "\n");
                break;
            }
        }

        // Clear all temporary results per line
        _res.tranArena.reset();

        // Handle Control Flow
        if (_ctx.jumpRequested){
            _ctx.jumpRequested = false;
            pc = _program.lower_bound(_ctx.jumpTarget);
            // We don't pc++ here, we start the next iteration at the jump target
        } else {
            pc++;
            if (limitToOneLine) break;
        }
    }
}

void BasicEngine::list(){
    for (auto const& line : _program) {
        _sys.console().print(std::to_string(line.first) + " " + line.second.source + "\n");
    }
}

void BasicEngine::clear() {
    _program.clear();
    _symTable.clear();
    _res.resetArenas();    // Wipes all arenas including Variable/Symbol space
    _res.resetVariables(); // Sets all variable slots to 0.0f
    _ctx.reset();          // Wipes stacks/PC
}

void BasicEngine::loadProgram(const std::string& script){
    _program.clear(); // Optional: clear existing program first
    _symTable.clear();
    _res.resetArenas();
    
    std::stringstream ss(script);
    std::string line;
    
    while (std::getline(ss, line)){
        if (line.empty()) continue;

        // Find the first space to separate line number from code
        size_t firstSpace = line.find(' ');
        if (firstSpace != std::string::npos){
            try{
                int lineNum = std::stoi(line.substr(0, firstSpace));
                std::string content = line.substr(firstSpace + 1);
                storeLine(lineNum, content);
            } catch (...){
                // Skip lines that don't start with a valid number
                continue;
            }
        }
    }
}

// Helper to wrap multiple statements into one if needed
StmtPtr BasicEngine::wrapStatements(std::vector<StmtPtr>&& statements){
    if (statements.size() == 1) {
        return std::move(statements[0]);
    }
    void* mem = _res.progArena.alloc(sizeof(BlockStmt));
    if (mem == nullptr) return nullptr;     // Explicit check before allocating

    return new (mem) BlockStmt(std::move(statements));
}

void BasicEngine::DumpProgram() {
    _sys.console().print("--- PROGRAM AST DUMP ---\n");

    PrettyPrinter printer(_symTable);

    for (auto const& [lineNum, lineData] : _program) {
        if (lineNum == 0) continue; // Skip immediate block
        
        _sys.console().print("[" + std::to_string(lineNum) + "] ");
        _sys.console().print(printer.print(lineData.statement) + "\n");
    }
    
    _sys.console().print("------------------------\n");
    _sys.console().print("Used:  " + std::to_string(reportProgramSize()) + " bytes\n");
    _sys.console().print("Free:  " + std::to_string(_res.progArena.getRemaining()) + " bytes\n");
}

void BasicEngine::reportParserError() {
    const auto& error = _parser.error();
    _sys.console().print("? PARSE ERROR");

    if (error.token.line > 0) {
        _sys.console().print(" ON LINE " + std::to_string(error.token.line));
    }

    if (error.message) _sys.console().print(": "); _sys.console().print(error.message);
    _sys.console().print("\n");
}