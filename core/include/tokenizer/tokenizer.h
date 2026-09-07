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

    /**
     * @brief tokenizes a cleaned input line
     * @param input Cleaned input
     * @return std::vector<Token> Vector of tokens in input order 
     */
    std::vector<Token>& tokenize(const std::string& source);
    

private:
    uint32_t _start = 0;     ///< Start of the current lexeme
    uint32_t _current = 0;   ///< Current char in the scanned lexeme
    uint32_t _line = 1;      ///< Tracking lines for error reporting

    util::StringView _source;
    std::vector<Token> _tokens; ///< List of tokens built by the tokenizer

    void scanToken(void);
    void reset();

    // Complexer lexemes
    void string();
    void number();
    void identifier();
    
    /*
        Helper Functions
    */
    char advance(){
        return _source.data[_current++];
    }

    bool isAtEnd(){
        return _current >= _source.length;
    }

    char peek(){
        if (isAtEnd()) return '\0';
        return _source.data[_current];
    }

    char peekNext(){
        if (_current + 1 >= _source.length) return '\0';
        return _source.data[_current + 1];
    }

    bool match(char expected){
        // Either break if its last char or if it doesnt match
        if (isAtEnd()) return false;
        if (_source.data[_current] != expected) return false;

        // If it does match, also consume it
        _current++;
        return true;
    }
    
    void addToken(TokenType type) {
        // For simple operators, value is empty or 1-char
        _tokens.emplace_back(type, util::StringView(_source.data + _start, _current - _start), _line);
    }

    void addToken(TokenType type, util::StringView value) {
        _tokens.emplace_back(type, value, _line);
    }

    bool isAlpha(char c){
        return std::isalpha(c) || c == '_';
    }

    bool isAlphaNumeric(char c){
        return isAlpha(c) || std::isdigit(c);
    }
};  