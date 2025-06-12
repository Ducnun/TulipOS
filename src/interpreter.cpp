#include "interpreter.h"

BasicInterpreter::BasicInterpreter(IOInterface &io) : _io(io), _executor(Executor(_io, _variables)) {}

void BasicInterpreter::runREPL(){
    // Setup
    _io.clearScreen();
    _io.print("**** TULIP 64 BASIC V2 ****\n");
    _io.print(" 64K RAM SYSTEM 38911 BASIC BYTES FREE\n");
    _io.print(" -- WELOME HOME WARRIOR -- \n");
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
        }
    }
}

std::string BasicInterpreter::preprocess(const std::string& line){
    std::string result;
    int charIndex = 0;

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

bool BasicInterpreter::handleREPLDirective(const std::string& line){    
    if (line == "DUMP"){
        for (const auto& varPair : _variables) {
            const std::string& varName = varPair.first;
            const Variable varValue = varPair.second.toString();

            _io.print(varName + " = " + varValue.toString() + "\n");
        }
        return true;
    }

    if (line == "BENCH"){
        _program.loadBenchmark();
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

    if (line == "CLEAR"){
        clearVariables();
        return true;
    }

    return false;
}

void BasicInterpreter::storeProgramLine(const std::string& line){
    std::istringstream iss(line);
    int lineNumber;
    iss >> lineNumber;
    
    std::string rest;
    std::getline(iss, rest);
    std::string cleanedRest = preprocess(rest);
    
    if (cleanedRest.empty()){
        _program.deleteLine(lineNumber);
    }else{
        _program.insertLine(lineNumber, cleanedRest);
    }
}

void BasicInterpreter::evaluateImmediateCommand(const std::string& line){
    Tokenizer tokenizer;
    std::vector<Token> tokens = tokenizer.tokenize(line);
    Parser parser(tokens);
    ParsedStatement stmt = parser.parse();
    _executor.execute(stmt);
}

void BasicInterpreter::runProgram(){
    _program.resetProgramCounter();
    bool running = true;

    while (running){
        _io.pollKeyboard();
        int currentLine = _program.getProgramCounter();

        if (currentLine == 0){
            _io.print("?UNDEF'D STATEMENT ERROR\n");
            _io.print("READY.\n");
            running = false;
            continue;
        }

        if (_io.checkBreakRequested()){     // checkBreakRequested consumes the break flag
            _io.print("BREAK IN " + std::to_string(currentLine) + "\n");
            _io.print("READY.\n");
            running = false;
            continue;
        }

        ExecutionResult result = _executor.execute(_program.getNextStatement());
        
        if (result.action == ExecutionAction::Jump) _program.jumpTo(result.jumpTarget);
        if (result.action == ExecutionAction::Halt) running = false;        
    }

}

void BasicInterpreter::listProgram(){
    std::vector<std::string> lines = _program.getSource();
    if (lines.empty()) return;
    for (const std::string& line : lines){
        _io.print(line + "\n");
    }
}

void BasicInterpreter::clearProgram(){
    _program.clear();
}

void BasicInterpreter::clearVariables(){
    _variables.clear();
}