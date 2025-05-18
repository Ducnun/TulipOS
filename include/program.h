#pragma once

#include <string>
#include <map>
#include "tokenizer.h"
#include "parser.h"


class Program{
public:
    Program(): _programCounter(0) {}
    void insertLine(int lineNumber, const std::string& source);
    void deleteLine(int lineNumber);
    std::vector<std::string> getSource();
    
    void resetProgramCounter();

    void saveToSD(const std::string& filename);
    void loadFromSD(const std::string& filename);

private:
    std::map<int, ParsedStatement> _statements;
    std::map<int, std::string> _sourceLines;
    std::vector<int> _lineNumbers;
    int _programCounter;
    void rebuildLineNumbers();
};
