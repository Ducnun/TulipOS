#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include "token.h"

class Tokenizer {
public: 
    /**
     * @brief Destroy the Tokenizer object
     */
    ~Tokenizer() = default;

    // Initializes tokenizer at provided source
    void init(util::StringView source, int startLine) {
        _source = source;
        _cursor = 0;
        _line = startLine;
    }

    Token nextToken();

private:
    util::StringView _source;
    uint32_t _cursor = 0;
    int _line = 0;

    /*
        Helper Functions
    */
    void skipWhitespace(){
        while (!isAtEnd()){
            char c = _source.data[_cursor];
            if (c == ' ' || c == '\n' || c == '\r') _cursor++;
            else if (c == '\n') {
                _line++;
                _cursor++;
            }
            else break;
        }
    }

    char advance(){
        return _source.data[_cursor++];
    }

    bool isAtEnd(){
        return _cursor >= _source.length;
    }

    char peek(){
        if (isAtEnd()) return '\0';
        return _source.data[_cursor];
    }

    char peekNext(){
        if (_cursor + 1 >= _source.length) return '\0';
        return _source.data[_cursor + 1];
    }

    bool match(char expected){
        // Either break if its last char or if it doesnt match
        if (isAtEnd()) return false;
        if (_source.data[_cursor] != expected) return false;

        // If it does match, also consume it
        _cursor++;
        return true;
    }

    bool isDigit(char c){ 
        return c >= '0' && c <= '9'; 
    }

    bool isAlpha(char c){
        return std::isalpha(c) || c == '_';
    }

    bool isAlphaNumeric(char c){
        return isAlpha(c) || std::isdigit(c);
    }
};  