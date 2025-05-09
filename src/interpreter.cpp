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

        // Exit inputs
        if (cleaned == "EXIT" || cleaned == "QUIT"){
            _io.print("OK");
            break;
        }

        // Store as program line if line starts with digits
        if (isdigit(cleaned[0])) {
            storeProgramLine(cleaned);
            continue;
        }

        // Run stored program when called
        if (cleaned == "RUN"){
            for (const auto& [lineNumber, tokens] : _programLines){
                Parser parser(tokens, _io, _variables);
                parser.parse();
            }
            continue;
        }

        // List stored program lines
        if (cleaned == "LIST"){
            for (const auto& [lineNumber, line] : _programLines){
                _io.print(std::to_string(lineNumber) + " ");
                for (const auto& seg : line){
                    _io.print(seg.value + " ");
                }
                _io.print("\n");
            }
        }

        if (cleaned == "NEW"){
            _programLines.clear();
        }

        // Call tokenizer and parser to process input
        Tokenizer tokenizer;
        std::vector<Token> tokens = tokenizer.tokenize(cleaned);
        Parser parser(tokens, _io, _variables);
        parser.parse();
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