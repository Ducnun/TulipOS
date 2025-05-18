#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <algorithm>
#include "tokenizer.h"
#include "parser.h"
#include "executor.h"


class Program{
public:
    Program(): _programCounter(0) {}

    // Program management
    void insertLine(int lineNumber, const std::string& source);
    void deleteLine(int lineNumber);
    void clear();
    std::vector<std::string> getSource();
    
    // Flow
    void resetProgramCounter();
    ParsedStatement getNextStatement();
    void jumpTo(int target);

    // Storage
    void saveToSD(const std::string& filename);
    void loadFromSD(const std::string& filename);

private:
    std::map<int, ParsedStatement> _statements;
    std::map<int, std::string> _sourceLines;
    std::vector<int> _lineNumbers;
    int _programCounter;
    void rebuildLineNumbers();
};
