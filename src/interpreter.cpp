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

        // Call tokenizer and parser to process input
        Tokenizer tokenizer;
        std::vector<Token> tokens = tokenizer.tokenize(cleaned);
        Parser parser(tokens, _io);
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