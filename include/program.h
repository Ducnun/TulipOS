#pragma once

#include <string>
#include <map>
#include "tokenizer.h"
#include "parser.h"

class Program{
public:
    void insertLine(int lineNumber, const std::string& source);
    void deleteLine(int lineNumber);
    void listLines();
    void run(std::map<std::string, VariableValue>& variables);
    void saveToSD(const std::string& filename);
    void loadFromSD(const std::string& filename);

private:
    std::map<int, ParsedStatement> _statements;
    std::vector<int> _lineNumbers;
    void rebuildLineNumbers();
};
