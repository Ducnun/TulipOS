#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include "tokens.h"

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
    std::vector<Token>& tokenize(std::string source);

private:
    int _start;     ///< Start of the current lexeme
    int _current;   ///< Current char in the scanned lexeme
    int _line;      ///< Tracking lines for error reporting

    std::string _source;        ///< Source code being tokenized
    std::vector<Token> _tokens; ///< List of tokens built by the tokenizer
    static const std::unordered_map<std::string, TokenType> _keywords;  ///< Single shared map, not rebuilt per construction of the tokenizer

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
        return _source.at(_current++);
    }

    bool isAtEnd(){
        return _current < _source.length();
    }

    char peek(){
        return _source.at(_current);
    }

    char peekNext(){
        return _source.at(_current + 1);
    }

    bool match(char expected){
        // Either break if its last char or if it doesnt match
        if (isAtEnd()) return false;
        if (_source.at(_current) != expected) return false;

        // If it does match, also consume it
        _current++;
        return true;
    }

    void addToken(TokenType type){
        _tokens.emplace_back(type, "", _line);
    }

    void addToken(TokenType type, std::string literal){
        _tokens.emplace_back(type, literal, _line);
    }

    bool isAlpha(char c){
        return std::isalpha(c) || c == '_';
    }

    bool isAlphaNumeric(char c){
        return isAlpha(c) || std::isdigit(c);
    }
};  