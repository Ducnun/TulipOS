#include "program.h"

void Program::insertLine(int lineNumber, const std::string& source){
    if (source.empty()){
        // deleteLine(lineNumber);
        return;
    }

    Tokenizer tokenizer;
    std::vector<Token> tokens = tokenizer.tokenize(source);
    Parser parser(tokens);
    ParsedStatement stmt = parser.parse();
}