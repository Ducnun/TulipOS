#include "shell.h"

Shell::Shell(IOInterface &io) : _io(io) {}

void Shell::runREPL(){
    // Setup
    _io.reset();
    _io.clearScreen();
    _io.print("**** TULIP BASIC V1 ****\n");
    
    #ifdef ESP32
    size_t freeHeap = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t used = 0;

    std::string memLine = std::to_string(freeHeap / 1000) + "K BYTES FREE (" +
                          std::to_string(used) + "B USED)\n";

                          
    _io.print(memLine);
                
    #endif
    _io.print("READY.\n");

    while (true){
        // Start by checking break
        if (_io.checkBreakRequested()) _io.print("BREAK\n");

        // Get new line
        _io.print("");
        _io.print("> ");
        std::string line = _io.readLine();

        // Clean line, if empty, skip
        std::string cleaned = preprocess(line);
        if (cleaned.empty()) continue;

        // If input is a REPL directive, skip rest
        if (handleREPLDirective(cleaned)) continue;

        // TODO: Abstract program handling away to handleProgramDirectives to clean up runREPL

        // Store as program line if line starts with digits
        if (isdigit(cleaned[0])){
            storeProgramLine(cleaned);
            continue;
        }

        // Run stored program when called
        if (cleaned == "RUN"){
            runProgram();
            continue;
        }

        // List stored program lines
        if (cleaned == "LIST"){
            listProgram();
            continue;
        }

        // Clean program when command NEW is entered in the terminal
        if (cleaned == "NEW"){
            clearProgram();
            continue;
        }

        // Call tokenizer and parser to process input
        try {evaluateImmediateCommand(cleaned); }
        catch (std::runtime_error e){
            _io.print("\n ?");
            _io.print(e.what());
            _io.print("\n");
        }
    }
}

std::string Shell::preprocess(const std::string& line){
    std::string result;
    int charIndex = 0;
    // Maybe two cursor strategy to trim trailing whitespace, or slicing at the end

    while (line[charIndex] == ' ') charIndex++;
    
    while (charIndex < line.length()){
        if (line[charIndex] == '\n' || line[charIndex] == '\r') {
            charIndex++; 
            continue;
        }
        result += std::toupper(line[charIndex]);
        charIndex++;
    }
    return result;
}

bool Shell::handleREPLDirective(const std::string& line){    
    if (line == "BENCH"){

        return true;
    }

    if (line == "EXIT" || line == "QUIT"){
        _io.print("OK");
        return true;
    }

    if (line == "CLS"){
        _io.clearScreen();
        return true;
    }

    return false;
}

void Shell::storeProgramLine(const std::string& line){
    std::istringstream iss(line);
    int lineNumber;
    iss >> lineNumber;
    
    std::string rest;
    std::getline(iss, rest);
    std::string cleanedRest = preprocess(rest);
}

void Shell::evaluateImmediateCommand(const std::string& line){
    // Hotwired to pretty print for DEBUGGING
    Tokenizer tokenizer;
    std::vector<Token> tokens = tokenizer.tokenize(line);

    Parser parser;
    std::vector<StmtPtr> statements = parser.parse(tokens);

    // PrettyPrinter printer;
    // printer.print(statements);
    // _io.print(printer.result);

    Interpreter interpr(_ctx, _io);
    interpr.execute(statements);
}

void Shell::runProgram() {
    if (_program.isEmpty()) return;

    _ctx.reset(); // Clear variables and halted flag
    
    // PC (Program Counter) is an iterator to our map
    auto pc = _program.begin();
    Interpreter _interpreter(_ctx, _io);

    while (pc != _program.end() && !_ctx.halted) {
        try {
            // 1. Execute the current line
            if (pc->second.statement) {
                pc->second.statement->accept(_interpreter);
            }

            // 2. Advance to the next sequential line
            pc++;

        } catch (const GotoSignal& signal) {
            // Handle GOTO: Look up the new iterator
            pc = _program.find(signal.targetLine);
            
            if (pc == _program.end()) {
                _io.print("?UNDEF'D STATEMENT ERROR IN LINE " + std::to_string(signal.targetLine) + "\n");
                break;
            }
        } catch (const EndSignal& signal) {
            break;
        } catch (const std::runtime_error& e) {
            _io.print("\nRUNTIME ERROR: ");
            _io.print(e.what());
            _io.print("\n");
            break;
        }
    }
    
    _io.print("READY.\n");
}

void Shell::listProgram(){
    for (auto const& line : _program) {
        _io.print(std::to_string(line.first) + " " + line.second.source + "\n");
    }
}

void Shell::clearProgram(){
    _program.clear();
}