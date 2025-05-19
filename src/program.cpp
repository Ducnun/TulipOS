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
    _sourceLines[lineNumber] = std::to_string(lineNumber) + source;
    rebuildLineNumbers();
}

void Program::deleteLine(int lineNumber){
    _statements.erase(lineNumber);
    _sourceLines.erase(lineNumber);
    rebuildLineNumbers();
}

void Program::clear(){
    _statements.clear();
    _sourceLines.clear();
    _lineNumbers.clear();
    _programCounter = 0;
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
    for (const auto& stmt : _statements){
        _lineNumbers.push_back(stmt.first);
    }
}

void Program::resetProgramCounter(){
    _programCounter = _lineNumbers.front();
}

ParsedStatement Program::getNextStatement(){
    ParsedStatement stmt;

    if (_lineNumbers.empty()) {
        stmt.type = StatementType::End;
    }

    auto it = std::find(_lineNumbers.begin(), _lineNumbers.end(), _programCounter);
    if (it == _lineNumbers.end()) {
        stmt.type = StatementType::End;
    }
    else{
        stmt = _statements[*it];
        it++;
        _programCounter = (it != _lineNumbers.end()) ? *it : -1;
    }

    return stmt;
}


void Program::jumpTo(int targetLine){
    auto it = std::find(_lineNumbers.begin(), _lineNumbers.end(), targetLine);
    if (it == _lineNumbers.end()) {
        throw std::runtime_error("GOTO to undefined line: " + std::to_string(targetLine));
    }
    _programCounter = targetLine;
}