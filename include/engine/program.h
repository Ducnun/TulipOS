#pragma once

#include <map>
#include <string>
#include "ast/statement.h"

class Program{
public:
    // We store the AST node and the original source string for the LIST command
    struct ProgramLine{
        StmtPtr statement;
        std::string source;
    };

    typedef std::map<int, ProgramLine> LineMap;

    void addLine(int number, StmtPtr stmt, std::string sourceText){
        _lines[number] ={ std::move(stmt), sourceText };
    }

    void removeLine(int number){
        _lines.erase(number);
    }

    void clear(){
        _lines.clear();
    }

    bool isEmpty() const{
        return _lines.empty();
    }

    LineMap::const_iterator begin() const{ return _lines.begin(); }
    LineMap::const_iterator end() const{ return _lines.end(); }

    /**
     * Used by GOTO. Returns the iterator to the specific line, 
     * or end() if not found.
     */
    LineMap::const_iterator find(int number) const{
        return _lines.find(number);
    }

    LineMap::const_iterator lower_bound(int number) const{
        return _lines.lower_bound(number);
    }

private:
    LineMap _lines;
};