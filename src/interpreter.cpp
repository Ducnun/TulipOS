#include "interpreter.h"

BasicInterpreter::BasicInterpreter(IOInterface &io) : _io(io) {}

void BasicInterpreter::runREPL(){
    // Setup
    _io.clearScreen();
    _io.print("**** COMMODORE 64 BASIC V2 ****\n");
    _io.print(" 64K RAM SYSTEM 38911 BASIC BYTES FREE\n");
    _io.print("READY.\n");

    while (true){
        // Get new line
        _io.print("");
        _io.print("> ");
        std::string line = _io.readLine();
        
        // Clean line, if empty, skip
        std::string cleaned = preprocess(line);
        if (cleaned.empty()) continue;

        // If input is a REPL directive, skip rest
        if (handleREPLDirective) continue;

        // Store as program line if line starts with digits
        if (isdigit(cleaned[0])) {
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
        evaluateImmediateCommand(cleaned);
    }
}

std::string BasicInterpreter::preprocess(const std::string& line){
    std::string result;
    for (char c : line){
        if (c == '\n' || c == '\r') continue;
        result += std::toupper(c);
    }
    return result;
}

bool BasicInterpreter::handleREPLDirective(const std::string& line){
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
    
    Tokenizer tokenizer;
    std::vector<Token> tokens = tokenizer.tokenize(rest);
    _programLines[lineNumber] = tokens;
}

void BasicInterpreter::evaluateImmediateCommand(const std::string& line){
    Tokenizer tokenizer;
    std::vector<Token> tokens = tokenizer.tokenize(line);
    Parser parser(tokens, _io, _variables);
    parser.parse();
}

void BasicInterpreter::runProgram(){
    for (const auto& pair : _programLines){
        std::vector<Token> tokens = pair.second;
        Parser parser(tokens, _io, _variables);
        parser.parse();
    }
}

void BasicInterpreter::listProgram(){
    for (const auto& pair : _programLines){
            int lineNumber = pair.first;
            const auto& line = pair.second;
            _io.print(std::to_string(lineNumber) + " ");

            for (const auto& seg : line){
                _io.print(seg.value + " ");
            }
            _io.print("\n");
    }
}

void BasicInterpreter::clearProgram(){
    _programLines.clear();
}

void BasicInterpreter::clearVariables(){
    _variables.clear();
}