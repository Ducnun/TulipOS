#include "program.h"

void Program::insertLine(int lineNumber, const std::string& source){
    if (source.empty()){
        deleteLine(lineNumber);
        return;
    }

    Tokenizer tokenizer;
    std::vector<Token> tokens = tokenizer.tokenize(source);
    Parser parser(tokens);
    ParsedStatement stmt = parser.parse();
    stmt.lineNumber = lineNumber;
    _statements[lineNumber] = stmt;
    _sourceLines[lineNumber] = source;
    rebuildLineNumbers();
}

void Program::deleteLine(int lineNumber){
    _statements.erase(lineNumber);
    _sourceLines.erase(lineNumber);
    rebuildLineNumbers();
}

std::vector<std::string> Program::getSource(){
    std::vector<std::string> result;

    for (int line : _lineNumbers){
        auto it = _sourceLines.find(line);
        if (it != _sourceLines.end()){
            result.push_back(_sourceLines[line]);
        }
    }

    return result;
}

void Program::rebuildLineNumbers(){
    _lineNumbers.clear();
    for (const auto& [line, _] : _statements){
        _lineNumbers.push_back(line);
    }
}
